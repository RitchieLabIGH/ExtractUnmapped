#!/bin/bash

# CeCILL licence notice
# Author: Lorenzi Claudio ( claudio.lorenzi@igh.cnrs.fr ) and 
#         Barrier Sylvain ( sylvain.barriere@igh.cnrs.fr )
# This software is a computer program whose purpose is to analyse biological data.
#
# This software is governed by the CeCILL  license under French law and
# abiding by the rules of distribution of free software.  You can  use, 
# modify and/ or redistribute the software under the terms of the CeCILL
# license as circulated by CEA, CNRS and INRIA at the following URL
# "http://www.cecill.info". 

# As a counterpart to the access to the source code and  rights to copy,
# modify and redistribute granted by the license, users are provided only
# with a limited warranty  and the software's author,  the holder of the
# economic rights,  and the successive licensors  have only  limited
# liability. 

# In this respect, the user's attention is drawn to the risks associated
# with loading,  using,  modifying and/or developing or reproducing the
# software by the user in light of its specific status of free software,
# that may mean  that it is complicated to manipulate,  and  that  also
# therefore means  that it is reserved for developers  and  experienced
# professionals having in-depth computer knowledge. Users are therefore
# encouraged to load and test the software's suitability as regards their
# requirements in conditions enabling the security of their systems and/or 
# data to be ensured and,  more generally, to use and operate it in the 
# same conditions as regards security. 

# The fact that you are presently reading this means that you have had
# knowledge of the CeCILL license and that you accept its terms.



### functions
usage()
{
    echo $1
    echo -e "\nPreprocessing: extract unmapped reads from BAM files and compute the k-mer counts, using KMC3"
    echo -e "\nusage: \n\tpreprocess.sh -i input_file -o output_dir -k kmer_length [-K][-h][-t int][-r int][q][-m int][-c int][-h]\n"
    echo -e "\t-i|--input-file STR\t A tab separated value file containing in the first column the sample name, \n\t\t\t\t in the second the group and in the third the file location. \n\t\t\t\t This last can be local, http, ftp or SRR/ERR.\n\t\t\t\t If list, it has to be column (;) separated"
    echo -e "\n\t-o|--output-dir STR\t The output directory. If doesn't exists, it will be created. Default \"./preprocess/\""
    echo -e "\n\t-ol|--output-list STR\t A file to create the final matrix. Default \"./create_matrix.tsv\""
    echo -e "\n\t-f|--sam-flag STR\t The flags to use to extract the unmapped reads. "
    echo -e "\n\t-m|--min-counts INT\t Don't consider k-mer present less than the given threshold. Default 5"    
    echo -e "\n\t-c|--counter-val INT\t Kmc counter value. Suggest to keep the default 4294967295, that is also the maximum available."    
    echo -e "\n\t-k|--kmer-length INT\t The length of the k-mer. Default 31"
    echo -e "\n\t-l|--library-type [NULL|fr|rf|ff|rr]\n\t\t\t\t The type of stranded library. In case of presence of one or more \"r\" file, it will be converted to its complementary reverse."
    echo -e "\n\t-t|--threads INT\t Number of threads to use. Default 1"   
    echo -e "\n\t-r|--ram INT\t\t Max ram used by kmc in Gb. Default 12"
    echo -e "\n\t-K|--keep-files\t\t Keep the intermediate files."
    echo -e "\n\t-h|--help\t\t Show this help.\n\n"    
    [[ "$2" != "" ]] && exit $2
}

rc_read(){
    $1 $2 | seqkit seq -p -r -j $threads
}

print_info(){
    echo -e "Kmer Analysis Preprocessing"
    echo -e "\tInput: \t\t\t${input_file}"
    echo -e "\tOutput: \t\t${outputDir}"
    echo -e "\tLibrary type: \t\t${library_type}"    
    echo -e "\tKeep files: \t\t${keepFiles}"
    echo -e "\tMin Counts: \t\t${minCounts}"
    echo -e "\tKmer length: \t\t${kmer_len}"    
    echo -e "\tThreads: \t\t${threads}"
    echo -e "\tKMC Max memory: \t\t${maxRam}"
    echo -e "\tKMC counter max value: \t${kmcCounterVal}"
}

run_till_success(){
	prev_count_success="0"
    count_success="1"
    command_to_run=$@
	while [[ ${prev_count_success} -ne ${count_success} ]]; do
   	    echo -n "###[MESSAGE][$(date +%y-%m-%d-%H:%M:%S)] running $1 "
   	    prev_count_success=$count_success
        $command_to_run || count_success=$(( count_success + 1)) ;
        if [[ ${prev_count_success} -ne ${count_success} ]]; then
       		echo "failed. Trying again with attempt ${count_success}"
        else
        	echo "succeeded."
        fi
    done
}

### params

input_file="NONE"
outputDir=$(realpath ./preprocess/)
keepFiles="F"
minCounts="5"
kmer_len="31"
threads="1"
maxRam="12"
kmcCounterVal="4294967295"
samflag="4"
matrix_file=$(realpath ./create_matrix.tsv)

while [ "$1" != "" ]; do
    case $1 in
        -i | --input-file )     shift
                                input_file=$1
                                ;;
        -o | --output-dir )     shift
                                outputDir=$1
                                ;;
        -ol | --output-list )   shift
                                matrix_file=$1
                                ;;
        -k | --kmer-length )    shift
                                kmer_len=$1
                                ;;
        -t | --threads )        shift
                                threads=$1
                                ;;
        -r | --ram )            shift
                                maxRam=$1
                                ;;
        -K | --keep-files )     keepFiles="T"
                                ;;
        -l | --library-type )   shift
                                library_type=$1
                                ;;
        -q | --use-fastqc )     use_fastqc="T"
                                ;;

        -m | --min-counts )     shift
                                minCounts=$1
                                ;;
        -c | --counter-val )     shift
                                kmcCounterVal=$1
                                ;;
		-f | --flags)			shift
								samflag=$1
								;;	                                
        -h | --help )           usage "" 0
                                exit
                                ;;
        * )                     usage "Parameter $1 unrecognized" 1
    esac
    shift
done

### Check the arguments 

[[ "${input_file}" == "NONE" ]] &&  usage "ERROR! Give the input file using -i" 1 
[ ! -f "${input_file}" ] &&  usage "ERROR! Give a valid input file using -i" 1 


### Initial message 

echo "###[MESSAGE][$(date +%y-%m-%d-%H:%M:%S)] starting"
print_info


### Main

start_time=$(date +%s)

outputDir=$(realpath ${outputDir})
input_file=$(realpath ${input_file})
mkdir -p ${outputDir}
currDir=$(realpath ./ )


grep -v "^#" ${input_file} > ${input_file}.tmp
input_file_tmp=$(realpath ${input_file}.tmp)
cd ${outputDir}



while read line; do
    partial_time=$(date +%s)
    s_name=$(echo -e "${line}" | awk -F "\t" '{print $1}' )
    s_class=$(echo -e "${line}" | awk -F "\t" '{print $2}' )
    s_links=$(echo -e "${line}" | awk -F "\t" '{gsub(";", " ", $3); print $3}' )
    s_files=""
    o_files=""
    downloaded="F"
    mkdir -p ./${s_name} 
    cd ./${s_name}
    mkdir -p ./tmp_dir/
    for f in $s_links; do
        fname=$(echo $f | awk -F "/" '{print $NF}')
        if [[ "${f}" =~ ^(http|ftp).*$ ]]; then
            echo "###[MESSAGE][$(date +%y-%m-%d-%H:%M:%S)] DOWNLOADING ${fname}"
            mkdir -p ./fastq
            cd ./fastq
            run_till_success wget ${f} -O $fname -q 
            f=$(realpath ./${fname})
            cd ..
            downloaded="T"
        else  
            [ ! -f "${f}" ] &&  usage "ERROR! File $f not found!" 1 
        fi
        s_files="${s_files} ${f}"
    done
    f_files=""
    samflag= $(echo "${samflag}" | awk '/[0-9]+/ {print "-f " $1}')
    for f in $s_files ; do
        if [[ "${f}" =~ bam$ ]]; then
	    	echo "###[MESSAGE][$(date +%y-%m-%d-%H:%M:%S)] Extracting ${fname}"
			mkdir -p ./fastq
			cd ./fastq
			if [ ! -f ${fname}_0.fq ]; then
				samtools fastq $samflag -@ ${threads} -0 ${fname}_0.fq -1 ${fname}_1.fq -2 ${fname}_2.fq ${f} 
			fi
			f=""
			for fa in ${fname}_0.fq ${fname}_1.fq ${fname}_2.fq ; do
				if [ $(ls -s $fa | awk '{print $1}') -gt 0 ]; then
					f="${f} $(realpath ${fa})"
				fi
			done
	        cd ..
	    else 
	    	echo "###[MESSAGE][$(date +%y-%m-%d-%H:%M:%S)] ${fname} is not a BAM file and will be ignored"
	    	f=""
        fi
        f_files="${f_files} ${f}"
    done
    s_files=$(echo "$f_files" | xargs )
    ## detect if the first file is compressed
    read_files=$(echo ${s_files} | awk '{ if ( $1 ~/.gz$|.zip$/ ) { print "zcat" } else { print "cat" } }' )
    file_type=$($read_files ${s_files} | head -n 1 | awk 'NR==1 { if ($0 ~ /^>/) { print "fm" } else { print "fq" } }')
    echo ${s_files} | awk '{for (i=1; i<= NF; i++) { print $i } }' > ./tmp_dir/kmc_input
    prev_count_success="0"
    count_success="1"
    while [[ ${prev_count_success} -ne ${count_success} ]]; do
   	    echo -n "###[MESSAGE][$(date +%y-%m-%d-%H:%M:%S)] running KMC "
	    mkdir -p ./working_dir/
   	    prev_count_success=$count_success
        kmc -k${kmer_len} -t${threads} -m${maxRam} -cs${kmcCounterVal} -ci${minCounts} -b -${file_type} @./tmp_dir/kmc_input ./tmp_dir/tmp.kmc ./working_dir/ 2> ./tmp_dir/kmc.log.err > ./tmp_dir/kmc.log.out  || count_success=$(( count_success + 1)) ;
        rm -fr ./working_dir
        if [[ ${prev_count_success} -ne ${count_success} ]]; then
       		echo "failed. Trying again with attempt ${count_success}"
        else
        	echo "succeeded."
        fi
    done
    ls -s ./tmp_dir/tmp.kmc*
    ls -hs ./tmp_dir/tmp.kmc*
    echo "###[MESSAGE][$(date +%y-%m-%d-%H:%M:%S)] running KMC dump"    
	kmc_dump ./tmp_dir/tmp.kmc ./tmp_dir/tmp.txt
    echo "###[MESSAGE][$(date +%y-%m-%d-%H:%M:%S)] sorting the text file"    
	sort -k1 --parallel=${threads} ./tmp_dir/tmp.txt | awk '{print $1 "\t" $2}' > ./${s_name}.tsv
	count_file=$(realpath ./${s_name}.tsv)
    echo -e "${count_file}\t${s_name}\t${s_class}" > ./tmp_dir/kma.input	
    cat ./tmp_dir/kma.input >> $matrix_file
    if [[ "${keepFiles}" == "F" ]]; then
        rm -fr ./fasta ./fastq
    fi
    rm -fr ./tmp_dir/
    cd ..
    runtime=$(( $(date +%s) - partial_time ))
    echo "Done in $runtime"
done < ${input_file}.tmp

rm ${input_file_tmp}

runtime=$(( $(date +%s) - start_time ))

echo "###[MESSAGE][$(date +%y-%m-%d-%H:%M:%S)] completed in $runtime"


