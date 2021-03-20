# coding: utf-8
import main
partitioning = main.get_partitioning(main.sizes,main.partitioners)
main.print_partitioning(partitioning,'',18)
idx = main.get_indices(partitioning,[19,30,35,37])
print(idx)
