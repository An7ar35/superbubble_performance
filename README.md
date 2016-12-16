# Superbubble Performance 

Test of the different super-bubble identifications algorithms in 
genome sequencing

## Table of Contents
1. [Basic architecture diagram](#basic-architecture-diagram)
    1. [Description](#arch-description)
2. [Graph construction example](#graph-construction-example)
    1. [Generated graphs](#generated-graphs)
    2. [Generated DB Tables](#generated-tables)
3. [Superbubble Algorithms](#superbubble-algorithms)
5. [Platforms Supported](#platforms-supported)
6. [License](#license)


---
## Basic Pipeline Architecture <a name="basic-architecture-diagram"></a>

![Basic architecture diagram](docs/resources/uml/basic_pipeline.png "Basic overview")

### Description <a name="arch-description"></a>

- FASTA formatted sequencer reads get parsed into the GraphConstructor.
- The GraphConstructor create the graph from the reads and a K-mer length provided.
- The WeightedGraph data structure goes through the compressor to concatenate nodes
  where possible.
- The GraphIndexer creates an index of all the K-mers (nodes) in the graph and
  constructs a indexed WeightedGraph equivalent of the K-mer string graph.
- The GraphInexer sends the indexed K-mers and indexed Edges to the database for storage.
- Optionally both kmer graphs and indexed graph can be exported to Dot format.


---
## Graph construction example <a name="graph-construction-example"></a>

Using a single read sequence: "_a_long_long_long_time_"

Using K-mer length: __5__

### Graphs generated <a name="generated-graphs"></a>

| Original | Compressed | Indexed | Reconstructed |
|:---:|:---:|:---:|:---:|
| ![Original graph](docs/resources/img/stage1.png "") |![Compressed graph](docs/resources/img/stage2.png "") |![Indexed graph](docs/resources/img/stage3.png "") |![Reconstructed](docs/resources/img/stage4.png "") 

- __Original__: graph constructed from the sequencer reads.
- __Compressed__: graph where compressable K-mer node sequences were concatenated.
- __Indexed__: graph where all K-mer node strings were indexed
- __Reconstructed__: reconstructed graph from the index and edge tables in the database.

### Database tables <a name="generated-tables"></a>

- Index for the Kmer strings

| ID  | Kmer     |
|:---:|:---------|
|  0  | ong_lon  |
|  1  | ong_time |
|  2  | a_lon    |
|  3  | long_    |
|  4  | _long    |

- Edges

| FromID | To_ID | Weight |
|:------:|:-----:|:------:|
|    0   |   4   |    2   |
|    2   |   4   |    1   |
|    3   |   1   |    1   |
|    3   |   0   |    2   |
|    4   |   3   |    3   |

- Graphs table (lists all the graphs stored in the database)

| GraphID |  Name  |
|:-------:|:-------|
|    1    | Test01 |

The Edge and Kmer index tables are created by concatenating
the GraphID (_n_) with the type of table: '_edges_n' and '_kmers_n_'.

__E.g.__: For '_Test01_' above, the Edge table would be named '_edges_1_'
and the Kmer index table '_kmers_1_'.

---
## Superbubble Algorithms <a name="superbubble-algorithms"></a>

//TODO

---
## Platforms supported <a name="platforms-supported"></a>

The software is bundled with the components required from the EADlib library.

### Linux ツ ###
- Need a version of GCC with C++14 support (made with GCC 6.2.1)
- CMake 3.5

### Mac OSX ###

- See above.
- Homebrew might help. Untested.
 
### Windows ###

- Nope.

---
# License <a name="license"></a>

This software is released under the [__GNU General Public License 2__](https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html) license.

Please reference when used in project and/or research.