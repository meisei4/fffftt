#!/usr/bin/env bash
set -euo pipefail

project_root="$(cd "$(dirname "$0")" && pwd)"
log_directory="${1:-$project_root/logs}"
tmux_session_prefix="${2:-audio-spectrum-visualizer}"
tmux_start_gate="${tmux_session_prefix}-start"
capture_seconds="${TMUX_MATRIX_CAPTURE_SECONDS:-15}"
any_build_failed=0
successful_targets=()
result_lines=()

build_targets=(
  cool-gl33
  cool-gl11
  fftw-gl11
  cool-dc
  fftw-dc
  sh4zam-butterfly
)

targets=(
  "cool-gl33|$project_root/bin/cool_gl33|$log_directory/cool-gl33.log"
  "cool-gl11|$project_root/bin/cool_gl11|$log_directory/cool-gl11.log"
  "fftw-gl11|$project_root/bin/fftw_gl11|$log_directory/fftw-gl11.log"
  "cool-dc|$project_root/bin/cool_dc|$log_directory/cool-dc.log"
  "fftw-dc|$project_root/bin/fftw_dc|$log_directory/fftw-dc.log"
  "sh4zam-butterfly|$project_root/bin/sh4zam_butterfly|$log_directory/sh4zam-butterfly.log"
)

mkdir -p "$log_directory"
rm -f "$log_directory"/*.log

cleanup() {
  for entry in "${targets[@]}"; do
    name="${entry%%|*}"
    tmux kill-session -t "${tmux_session_prefix}-${name}" 2>/dev/null || true
  done
}
trap cleanup EXIT

for target in "${build_targets[@]}"; do
  if make -C "$project_root" "$target"; then
    result_lines+=("$(printf '%-17s [SUCCESS]' "$target")")
    successful_targets+=("$target")
  else
    result_lines+=("$(printf '%-17s [FAILED]' "$target")")
    any_build_failed=1
  fi
done

printf '\n'
printf '##################################\n'
printf '##      TMUX_MATRIX RESULTS     ##\n'
printf '##################################\n'
for result_line in "${result_lines[@]}"; do
  printf '%s\n' "$result_line"
done

for entry in "${targets[@]}"; do
  name="${entry%%|*}"
  should_launch=0
  for successful_target in "${successful_targets[@]}"; do
    if [ "$successful_target" = "$name" ]; then
      should_launch=1
      break
    fi
  done
  [ "$should_launch" -eq 1 ] || continue

  remainder="${entry#*|}"
  launch_command="${remainder%%|*}"
  log_file="${remainder#*|}"
  session_name="${tmux_session_prefix}-${name}"

  tmux kill-session -t "$session_name" 2>/dev/null || true
  tmux new-session -d -s "$session_name" \
    "tmux wait-for '$tmux_start_gate' && cd '$project_root' && FLYCAST_LOG_PATH='$log_directory/flycast-$name.log' $launch_command >'$log_file' 2>&1"
done

if [ "${#successful_targets[@]}" -eq 0 ]; then
  exit 1
fi

sleep 1
tmux wait-for -S "$tmux_start_gate"
sleep "$capture_seconds"

exit "$any_build_failed"
