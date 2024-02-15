# check if the number of arguments is correct
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <tab_number[1:17]> <verbose{1,2,3}>"
    exit 1
fi

cd build
cmake ../src/
make
./vrp $1 $2

