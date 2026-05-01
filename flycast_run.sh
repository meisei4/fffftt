#!/usr/bin/env bash
set -euo pipefail

target_elf_path="${1:?Usage: flycast_run.sh <elf_path> [emulator_args...]}"
shift || true

cd "$(dirname "$0")"
log_file_path="${FLYCAST_LOG_PATH:-logs/flycast_run.log}"
mkdir -p logs

emu_bin="${FLYCAST_BIN:-flycast}"
emu_extra_args="${FLYCAST_EXTRA_ARGS:-}"
window_width="${FLYCAST_WINDOW_WIDTH:-640}"
window_height="${FLYCAST_WINDOW_HEIGHT:-480}"
render_resolution="${FLYCAST_RENDER_RESOLUTION:-480}"

flycast_config_args=(
  -config "window:fullscreen=no"
  -config "window:maximized=no"
  -config "window:width=${window_width}"
  -config "window:height=${window_height}"
  -config "config:rend.Resolution=${render_resolution}"
)

SDL_VIDEO_HIGHDPI_DISABLED="${SDL_VIDEO_HIGHDPI_DISABLED:-1}" \
  $emu_bin $emu_extra_args "${flycast_config_args[@]}" "$@" "${target_elf_path}" > "$log_file_path" 2>&1 &
flycast_process_id=$!
tail -f "$log_file_path" &
log_tail_process_id=$!

while kill -0 "$flycast_process_id" 2>/dev/null; do
  serial_tty_path=$(awk 'match($0, /\/dev\/[^ ]+/) { print substr($0, RSTART, RLENGTH); exit }' "$log_file_path" 2>/dev/null)
  if [ -n "$serial_tty_path" ] && [ -c "$serial_tty_path" ]; then
    kill $log_tail_process_id 2>/dev/null || true
    wait $log_tail_process_id 2>/dev/null || true
    cat "$serial_tty_path" | tee -a "$log_file_path" &
    log_tail_process_id=$!
    break
  fi
  sleep 0.1
done

wait $flycast_process_id 2>/dev/null || true
kill $log_tail_process_id 2>/dev/null || true
wait $log_tail_process_id 2>/dev/null || true
