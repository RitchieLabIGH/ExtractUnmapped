# ExtractUnmapped

Extract informative k-mers from unmapped sequences.

[![https://www.singularity-hub.org/static/img/hosted-singularity--hub-%23e32929.svg](https://www.singularity-hub.org/static/img/hosted-singularity--hub-%23e32929.svg)](https://singularity-hub.org/collections/4065)


## Installation
The software is included in a [Singularity](https://sylabs.io/docs/) image. To use it you just need to install Singularity, or load the corresponding module if you are working on a cluster, and pull the image:
```bash
singularity pull shub://RitchieLabIGH/ExtractUnmapped 
```
This will create a file, ExtractUnmapped.sif, that contains the software and the whole environment.

## Usage
The software identify the k-mers whose aboundance can classify the samples belonging to two or more different groups. The methods uses a Bayesian Classifier, implemented in the library mlpack3.
### Unmapped read extraction
As first step is necessary to extract the unmapped reads from the bam files your samples. 
The image includes a script, preprocess.sh, that performs automatically this first step.
```bash
singularity exec ExtractUnmapped.sif preprocess.sh \
	-i samples.tsv \
	-o ./preprocessed \
	-ol ./create_matrix.tsv \
	-r 8 \
	-t 4
```
You can have a full list of arguments with the corresponding description with 
```bash
singularity exec ExtractUnmapped.sif preprocess.sh -h
```
The input file, in the exaple samples.tsv, has to be a tab separated value file with three columns:

 1. Sample name
 2. Sample group
 3. BAM file location

```tsv
sample1	responsive	/home/user/data/sample1.bam
sample2	unresponsive	/home/user/data/sample2.bam
...
```

### Matrix creation
The following step requires to build a matrix header, that will allow the software to retrieve all the columns for the reduction process.

```bash
singularity exec ExtractUnmapped.sif ExtractUnmapped matrix \
	-i ./create_matrix.tsv
	-o ./matrix.json
```


### Matrix reduction
The last step will analyse k-mer by k-mer to identify those that can be used to classify the samples in the different classes using a Bayes classsifier. The metric used for the evaluation is the accuracy ( number of correct classification divided by the total number of test ) using on a Monte Carlo cross validation.

```bash
singularity exec ExtractUnmapped.sif ExtractUnmapped reduce \
	-i ./matrix.json \
	-o ./reduced_matrix.tsv
```

Additional arguments are available to tune the cross validation step and the accuracy threshold. To have a detailed list, use:
  
```bash
singularity exec ExtractUnmapped.sif ExtractUnmapped reduce -h
```
The output file is a tab separated value table with the name of the k-mer as first column and one column for each pairwise comparison's accuracies. 
For example if you have 3 groups A, B and C, the output file will have the following header:
```tsv
kmer	A_x_B	A_x_C	B_x_C
```



