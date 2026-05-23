#!/usr/bin/env bash
set -euo pipefail

project_root="$(cd "$(dirname "$0")" && pwd)"
log_directory="$project_root/logs"
tmux_session_prefix="fffftt"
tmux_start_gate="${tmux_session_prefix}-start"
capture_seconds=15
build_jobs=8

desktop_build=false
if [ "$#" -gt 0 ] && [ "$1" = "desktop" ]; then
  desktop_build=true
fi

build_targets=(
  dc-sh4zam-butterfly
  dc-picking-out-notes
  dc-waveform
  dc-sound-envelope-2d-iso
  dc-sound-envelope-3d
  dc-waveform-terrain-3d
  dc-fft-terrain-3d
  dc-fft-bands-terrain-3d
)

run_targets=(
  "dc-sh4zam-butterfly|$project_root/bin/dc/sh4zam-butterfly|$log_directory/dc-sh4zam-butterfly.log"
  "dc-picking-out-notes|$project_root/bin/dc/picking-out-notes|$log_directory/dc-picking-out-notes.log"
  "dc-waveform|$project_root/bin/dc/waveform|$log_directory/dc-waveform.log"
  "dc-sound-envelope-2d-iso|$project_root/bin/dc/sound-envelope-2d-iso|$log_directory/dc-sound-envelope-2d-iso.log"
  "dc-sound-envelope-3d|$project_root/bin/dc/sound-envelope-3d|$log_directory/dc-sound-envelope-3d.log"
  "dc-waveform-terrain-3d|$project_root/bin/dc/waveform-terrain-3d|$log_directory/dc-waveform-terrain-3d.log"
  "dc-fft-terrain-3d|$project_root/bin/dc/fft-terrain-3d|$log_directory/dc-fft-terrain-3d.log"
  "dc-fft-bands-terrain-3d|$project_root/bin/dc/fft-bands-terrain-3d|$log_directory/dc-fft-bands-terrain-3d.log"
)

if [ "$desktop_build" = true ]; then
  build_targets=(
    desktop-sh4zam-butterfly
    desktop-picking-out-notes
    desktop-waveform
    desktop-sound-envelope-2d-iso
    desktop-sound-envelope-3d
    desktop-waveform-terrain-3d
    desktop-fft-terrain-3d
    desktop-fft-bands-terrain-3d
  )

  run_targets=(
    "desktop-sh4zam-butterfly|$project_root/bin/desktop/sh4zam-butterfly|$log_directory/desktop-sh4zam-butterfly.log"
    "desktop-picking-out-notes|$project_root/bin/desktop/picking-out-notes|$log_directory/desktop-picking-out-notes.log"
    "desktop-waveform|$project_root/bin/desktop/waveform|$log_directory/desktop-waveform.log"
    "desktop-sound-envelope-2d-iso|$project_root/bin/desktop/sound-envelope-2d-iso|$log_directory/desktop-sound-envelope-2d-iso.log"
    "desktop-sound-envelope-3d|$project_root/bin/desktop/sound-envelope-3d|$log_directory/desktop-sound-envelope-3d.log"
    "desktop-waveform-terrain-3d|$project_root/bin/desktop/waveform-terrain-3d|$log_directory/desktop-waveform-terrain-3d.log"
    "desktop-fft-terrain-3d|$project_root/bin/desktop/fft-terrain-3d|$log_directory/desktop-fft-terrain-3d.log"
    "desktop-fft-bands-terrain-3d|$project_root/bin/desktop/fft-bands-terrain-3d|$log_directory/desktop-fft-bands-terrain-3d.log"
  )
fi

cleanup() {
  for entry in "${run_targets[@]}"; do
    target_name="${entry%%|*}"
    tmux kill-session -t "${tmux_session_prefix}-${target_name}" 2>/dev/null || true
  done
}

trap cleanup EXIT

mkdir -p "$log_directory"
rm -f "$log_directory"/*.log

make -C "$project_root" clean-all
make -C "$project_root" -k -j "$build_jobs" "${build_targets[@]}"

for entry in "${run_targets[@]}"; do
  target_name="${entry%%|*}"
  remainder="${entry#*|}"
  run_command="${remainder%%|*}"
  log_file="${remainder#*|}"
  session_name="${tmux_session_prefix}-${target_name}"

  tmux kill-session -t "$session_name" 2>/dev/null || true
  tmux new-session -d -s "$session_name" \
    "tmux wait-for '$tmux_start_gate' && cd '$project_root' && FLYCAST_LOG_PATH='$log_directory/flycast-$target_name.log' '$run_command' >'$log_file' 2>&1"
done

sleep 1
tmux wait-for -S "$tmux_start_gate"
sleep "$capture_seconds"