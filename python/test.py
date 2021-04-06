#!/usr/bin/env python3
import main
print("test1")
partitioning = main.get_partitioning(main.sizes,main.partitioners_list)
print("test1")
main.print_partitioning(partitioning,'',18)
print("test1")
idx = main.get_indices(partitioning,[19,30,35,37])
print(idx)
