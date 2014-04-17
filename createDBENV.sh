 #!/bin/bash

export LD_LIBRARY_PATH=../../lib/

echo "creating DB env..."

export PGHOST=localhost
export PGDATABASE=anakin-test
export PGUSER=anakin
export PGPASSWORD=padme

echo "DB env created"
