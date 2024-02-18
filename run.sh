# check if the number of arguments is one or two
if [ -z "$1" ]; then
    echo "Usage: $0 <tab_number[1:17]> (verbose{0,1,2})"
    exit 1
fi

# default value for $2 is 0
default=0
if [ -n "$2" ]; then
    default=$2
fi

cd build
cmake ../src/
make
./vrp $1 $default

