#!/usr/bin/env bash
set -euo pipefail

project_root="$(cd "$(dirname "$0")" && pwd)"
log_directory="${1:-$project_root/logs}"
tmux_session_prefix="${2:-fffftt}"
tmux_start_gate="${tmux_session_prefix}-start"
capture_seconds="${BUILD_CAPTURE_SECONDS:-15}"
build_jobs="${BUILD_JOBS:-$(getconf _NPROCESSORS_ONLN 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 8)}"

build_targets=(
  sh4zam-butterfly
  picking-out-notes-dc
  waveform-dc
  sound-envelope-dc
  sound-envelope-3d-dc
  waveform-terrain-3d-dc
  fft-terrain-3d-dc
  fft-bands-terrain-3d-dc
)

run_targets=(
  "sh4zam-butterfly|$project_root/bin/sh4zam_butterfly|$log_directory/sh4zam-butterfly.log"
  "picking-out-notes-dc|$project_root/bin/picking_out_notes_dc|$log_directory/picking-out-notes-dc.log"
  "waveform-dc|$project_root/bin/waveform_dc|$log_directory/waveform-dc.log"
  "sound-envelope-dc|$project_root/bin/sound_envelope_dc|$log_directory/sound-envelope-dc.log"
  "sound-envelope-3d-dc|$project_root/bin/sound_envelope_3d_dc|$log_directory/sound-envelope-3d-dc.log"
  "waveform-terrain-3d-dc|$project_root/bin/waveform_terrain_3d_dc|$log_directory/waveform-terrain-3d-dc.log"
  "fft-terrain-3d-dc|$project_root/bin/fft_terrain_3d_dc|$log_directory/fft-terrain-3d-dc.log"
  "fft-bands-terrain-3d-dc|$project_root/bin/fft_bands_terrain_3d_dc|$log_directory/fft-bands-terrain-3d-dc.log"
)

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