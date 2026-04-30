#!/usr/bin/env bash
set -euo pipefail

project_root="$(cd "$(dirname "$0")" && pwd)"
log_directory="${1:-$project_root/logs}"
tmux_session_prefix="${2:-sound-envelope}"
tmux_start_gate="${tmux_session_prefix}-start-$$"
capture_seconds="${TMUX_SOUND_ENVELOPE_CAPTURE_SECONDS:-15}"
any_build_failed=0
successful_targets=()
result_lines=()

targets=(
  "gl33|sound-envelope-gl33|$project_root/bin/sound_envelope_gl33|$log_directory/sound-envelope-gl33.log"
  "dc|sound-envelope-dc|$project_root/bin/sound_envelope_dc|$log_directory/sound-envelope-dc.log"
  "3d-dc|sound-envelope-3d-dc|$project_root/bin/sound_envelope_3d_dc|$log_directory/sound-envelope-3d-dc.log"
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

for entry in "${targets[@]}"; do
  IFS='|' read -r name build_target launcher_path log_file <<< "$entry"

  if make -C "$project_root" "$build_target"; then
    result_lines+=("$(printf '%-24s [SUCCESS]' "$build_target")")
    successful_targets+=("$name")
  else
    result_lines+=("$(printf '%-24s [FAILED]' "$build_target")")
    any_build_failed=1
    continue
  fi

  tmux kill-session -t "${tmux_session_prefix}-${name}" 2>/dev/null || true
  tmux new-session -d -s "${tmux_session_prefix}-${name}" \
    "tmux wait-for '$tmux_start_gate' && cd '$project_root' && FLYCAST_LOG_PATH='$log_directory/flycast-$name.log' '$launcher_path' >'$log_file' 2>&1"
done

printf '\n'
printf '########################################\n'
printf '##  TMUX_SOUND_ENVELOPE_MATRIX RESULTS ##\n'
printf '########################################\n'
for result_line in "${result_lines[@]}"; do
  printf '%s\n' "$result_line"
done
printf '\n'

if [ "${#successful_targets[@]}" -eq 0 ]; then
  exit 1
fi

sleep 1
tmux wait-for -S "$tmux_start_gate"
sleep "$capture_seconds"

exit "$any_build_failed"
