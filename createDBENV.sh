 #!/bin/bash

export LD_LIBRARY_PATH=../../lib/

echo "creating DB env..."

export PGHOST=localhost
export PGDATABASE=Anakin
export PGUSER=postgres
export PGPASSWORD=postgres

echo "DB env created"
