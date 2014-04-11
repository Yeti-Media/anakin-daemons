 #!/bin/bash

export LD_LIBRARY_PATH=/home/franco/Trabajos/Yeti-Media/Git/Proyecto/anakin-daemons/lib/

echo "creating DB env..."

export PGHOST=localhost
export PGDATABASE=postgres
export PGUSER=postgres
export PGPASSWORD=padme

echo "DB env created"
