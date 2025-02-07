cmake_minimum_required(VERSION 3.28.3)

project(txtfs VERSION 4.0)

include_directories(
    "src"
)

add_executable(txtfs
    src/Vector.h
    src/UniqueId.h
    src/Set.h
    src/Sequence.h
    src/PoolAllocator.h
    src/ISet.h
    src/ISequence.h
    src/IDictionary.h
    src/FSTree.h
    src/FSCli.h
    src/Dictionary.h
    src/AVLTree.h
    main.cpp
)