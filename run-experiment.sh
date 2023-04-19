mkdir -m 777 build
(cd ./build && cmake -DCMAKE_BUILD_TYPE=Release .. && make)

# Run experiments
(cd ./build/experiments && ./exp)

# Graph experiments
python3 ./experiments/drawGraphs.py

