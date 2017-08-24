#!/bin/sh

get_filename()
{
  for idx in $(seq 1 1000000)
  do
    local filename="result.${idx}"
    if [ ! -f "${filename}" ]; then
      touch "${filename}"
      echo "${filename}"
      return 0
    fi
  done
  exit 1
}

runner()
{
  local exe_path="$1"
  local filename="$2"
  echo "Starting a process '${exe_path}' to file '${filename}'"
  while true; do
    "$1" 1 100000 >> "${filename}" 2>&1
    sleep 1
  done
}

pid_file="pid.file"

main()
{
  cp ../cards.json .

  local exe_path="$1"
  local processes=2
  local filename
  for idx in $(seq 1 $processes); do
    filename="$(get_filename)"
    runner "${exe_path}" "${filename}" &
    child_pid="$!"
    echo "Started a process ${child_pid}"
    echo "${child_pid}" >> "${pid_file}"
  done

  sleep 1
  echo "Entering an infinite loop to wait forever..."
  while true; do
    sleep 1
  done
}

finish()
{
  for pid in `cat ${pid_file}`; do
    echo "Killing child process ${pid}"
    kill -9 ${pid}
  done
  rm -f ${pid_file}
  exit 0
}

trap finish INT

main "$@"
