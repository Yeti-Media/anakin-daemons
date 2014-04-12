 #!/bin/bash

echo "creating DB env..."

export PGHOST=localhost
export PGDATABASE=postgres
export PGUSER=postgres
export PGPASSWORD=padme

echo "DB env created"
