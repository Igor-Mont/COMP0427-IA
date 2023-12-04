#!/bin/bash

# Set the base path to search
base_path="/home/pedrov/repos/UFS/ia/COMP0427-IA/cpp"

# Find all files ending with _test.cpp
find "${base_path}" -type f -name '*_test.cpp' -exec bash -c '
    filepath="$1"

    # Check if the file is empty
    if [ ! -s "${filepath}" ]; then
        # File is empty, append the required lines
        echo "#define CATCH_CONFIG_MAIN" >> "${filepath}"
        echo '#include "../../catch.hpp"' >> "${filepath}"
        echo "#include \"$(basename "${filepath}" _test.cpp).cpp\"" >> "${filepath}"
    fi
' bash {} \;

