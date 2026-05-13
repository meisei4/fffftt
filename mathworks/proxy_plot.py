from dataclasses import dataclass
import os
from pathlib import Path
import wave

import numpy as np
import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt
from matplotlib.colors import LinearSegmentedColormap, to_rgba
from matplotlib.ticker import LinearLocator
from matplotlib import font_manager
from mpl_toolkits.mplot3d.axes3d import Axes3D
from mpl_toolkits.mplot3d.art3d import Poly3DCollection

ROOT = Path(__file__).resolve().parent

PROJECTION = "ortho"
VIEW_ELEVATION = 28
VIEW_AZIMUTH = -56
BOX_ASPECT = (1.50, 3.20, 0.65)
VIEW_ZOOM = 1.35

FIGURE_WIDTH = 8.0
FIGURE_HEIGHT = 6.0
FIGURE_DPI = 200

PADDING_INCHES = 0.01

VGA_FONT_PATH = ROOT / "vga.ttf"
if VGA_FONT_PATH.exists():
    font_manager.fontManager.addfont(str(VGA_FONT_PATH))

FONT_FAMILY = font_manager.FontProperties(fname=str(VGA_FONT_PATH) if VGA_FONT_PATH.exists() else None).get_name()
FONT_FALLBACKS = []
AXIS_LABEL_SIZE = 8.5
TICK_LABEL_SIZE = 7.5
TITLE_SIZE = 10.0
TICK_PADDING = 0
AXIS_LABEL_PADDING_X = 5
AXIS_LABEL_PADDING_Y = 14
AXIS_LABEL_PADDING_Z = 7

TICK_DIRECTION = "out"
TICK_LENGTH = 3
TICK_LINEWIDTH_TICK = 0.4
AXIS3D_TICK_INWARD = 0.16
AXIS3D_TICK_OUTWARD = 0.05
X_TICK_COUNT = 6
Y_TICK_COUNT = 7
Z_TICK_COUNT = 5
FRAME_LINE_WIDTH = 0.4
SURFACE_EDGE_LINEWIDTH = 0.035
WIREFRAME_PRIMARY_LINEWIDTH = 0.38
WIREFRAME_SECONDARY_LINEWIDTH = 0.25
ANTIALIASED = False

BACKGROUND_COLOR = "#000000"
FRAME_COLOR = "#ffffff"
TEXT_COLOR = "#d6e6ff"

SAMPLE_RATE = 22050
INPUT_WAV_FILE = ROOT / "sketch_target.wav"

FFT_X_LABEL = "Freq"
FFT_Y_LABEL = "Time"
FFT_Z_LABEL = ""
FFT_HEIGHT_SCALE = 2.65
FFT_Z_MIN = 0.0
FFT_Z_MAX = 2.4

WAVEFORM_X_LABEL = ""
WAVEFORM_Y_LABEL = "Time"
WAVEFORM_Z_LABEL = ""
WAVEFORM_HEIGHT_SCALE = 1.55
WAVEFORM_Z_MIN = 0.0
WAVEFORM_Z_MAX = 1.2


BAND_X_LABEL = ""
BAND_Y_LABEL = ""
BAND_Z_LABEL = ""
LOW_BAND_HEIGHT_SCALE = 1.15
MID_BAND_HEIGHT_SCALE = 1.55
HIGH_BAND_HEIGHT_SCALE = 1.15
LOW_BAND_POINT_COUNT = 12
MID_BAND_POINT_COUNT = 24
HIGH_BAND_POINT_COUNT = 19
BAND_GAP = 2

MERGED_X_LABEL = ""
MERGED_Y_LABEL = "Time"
MERGED_Z_LABEL = ""
MERGED_LOW_SCALE = 1.25
MERGED_MID_SCALE = 2.0
MERGED_HIGH_SCALE = 1.25

SMALL_TICK_LABEL_SIZE = 4.8
SMALL_TICK_PADDING = 0
SMALL_AXIS_LABEL_SIZE = 5.2
SMALL_AXIS_LABEL_PADDING_X = 4
SMALL_AXIS_LABEL_PADDING_Y = 12
SMALL_AXIS_LABEL_PADDING_Z = 6
SMALL_X_TICK_COUNT = 6
SMALL_Y_TICK_COUNT = 7
SMALL_Z_TICK_COUNT = 5
SMALL_SURFACE_EDGE_LINEWIDTH = 0.028
SMALL_WIREFRAME_ROW_SKIP = 10
SMALL_WIREFRAME_COLUMN_SKIP = 8
SMALL_WIREFRAME_PRIMARY_LINEWIDTH = 0.30
SMALL_WIREFRAME_SECONDARY_LINEWIDTH = 0.20

WIREFRAME_ROW_SKIP = 14
WIREFRAME_COLUMN_SKIP = 11

FFT_WINDOW_SIZE = 1024
HOP_SIZE = 2048
FFT_BINS = 512
DECIBEL_MINIMUM = -100.0
DECIBEL_MAXIMUM = -30.0
SMOOTHING_FACTOR = 0.8
TOTAL_LANES = 112
WAVEFORM_POINTS = 96
FFT_POINTS = 96
RUNTIME_POINTS = 33
DECIBEL_SCALE_FACTOR = 20.0 / np.log(10.0)
CHROMA_COUNT = 12
CHROMA_A0_HZ = 27.5
CHROMA_INVERSE_LN_2 = 1.0 / np.log(2.0)
ACCIDENTAL_GAIN = np.array([1.0, 0.5, 1.0, 0.5, 1.0, 1.0, 0.5, 1.0, 0.5, 1.0, 0.5, 1.0], dtype=np.float32)
ADAPTIVE_ONSET_RATE = 0.020
ADAPTIVE_ONSET_GATE_DEVIATION_SCALE = 0.75
ADAPTIVE_ONSET_RANGE_DEVIATION_SCALE = 3.00
GLOBAL_ADAPTIVE_GATE_SMOOTHING_FACTOR = 0.08
GLOBAL_ADAPTIVE_GATE_FLOOR_DEVIATION_SCALE = 0.75
GLOBAL_ADAPTIVE_GATE_CEILING_DEVIATION_SCALE = 1.25
ONSET_DECAY_RATE = 0.35
ONSET_DELAY_FRAMES = 1
ONSET_ENVELOPE_RADIUS = 2
LIGHT0_DIFFUSE_MAX = 1.5
VISIBLE_ONSET_MIN_GATE = 0.18
VISIBLE_ONSET_MERGE_FRAMES = 5
CURTAIN_MIN_GATE = 0.18
CURTAIN_MERGE_FRAMES = 8
CURTAIN_MAX_EVENTS_VISIBLE = 0
ONSET_WASH_DISPLAY_ROWS = 320
ONSET_EVENT_BASELINE_KERNEL = np.array([1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2, 1], dtype=np.float32)
ONSET_EVENT_REFRACTORY_FRAMES = 5

LOW_BAND_BOUNDS = [(i, i) for i in range(1, 13)]
MID_BAND_BOUNDS = [
    (13, 13),
    (14, 15),
    (16, 17),
    (18, 19),
    (20, 21),
    (22, 24),
    (25, 27),
    (28, 30),
    (31, 34),
    (35, 38),
    (39, 43),
    (44, 48),
    (49, 53),
    (54, 60),
    (61, 67),
    (68, 75),
    (76, 84),
    (85, 94),
    (95, 105),
    (106, 118),
    (119, 132),
    (133, 148),
    (149, 165),
    (166, 185),
]
HIGH_BAND_BOUNDS = [
    (186, 202),
    (203, 219),
    (220, 236),
    (237, 253),
    (254, 270),
    (271, 287),
    (288, 305),
    (306, 322),
    (323, 339),
    (340, 356),
    (357, 373),
    (374, 390),
    (391, 408),
    (409, 425),
    (426, 442),
    (443, 459),
    (460, 476),
    (477, 493),
    (494, 511),
]


def apply_matlab5_rcparams():
    plt.rcParams.update(
        {
            "font.family": FONT_FAMILY,
            "font.monospace": [FONT_FAMILY] + FONT_FALLBACKS,
            "font.size": TICK_LABEL_SIZE,
            "axes.labelsize": AXIS_LABEL_SIZE,
            "axes.titlesize": TITLE_SIZE,
            "xtick.labelsize": TICK_LABEL_SIZE,
            "ytick.labelsize": TICK_LABEL_SIZE,
            "xtick.direction": TICK_DIRECTION,
            "ytick.direction": TICK_DIRECTION,
            "axes.linewidth": FRAME_LINE_WIDTH,
            "grid.linewidth": FRAME_LINE_WIDTH,
            "lines.antialiased": ANTIALIASED,
            "text.antialiased": ANTIALIASED,
            "axes.unicode_minus": False,
        }
    )


apply_matlab5_rcparams()


def _apply_matplotlib_3d_defaults() -> None:
    if getattr(Axes3D, "_mathworks_3d_defaults_applied", False):
        return

    original_plot = Axes3D.plot
    original_plot_surface = Axes3D.plot_surface
    original_scatter = Axes3D.scatter
    original_add_collection3d = Axes3D.add_collection3d

    def apply_artist_clip_defaults(artist):
        if artist is None:
            return artist
        if isinstance(artist, (list, tuple)):
            for item in artist:
                apply_artist_clip_defaults(item)
            return artist
        artist.set_clip_on(False)
        artist.set_clip_box(None)
        artist.set_clip_path(None)
        return artist

    def plot_with_defaults(self, *args, **kwargs):
        kwargs.setdefault("axlim_clip", False)
        return apply_artist_clip_defaults(original_plot(self, *args, **kwargs))

    def plot_surface_with_defaults(self, *args, **kwargs):
        kwargs.setdefault("axlim_clip", False)
        return apply_artist_clip_defaults(original_plot_surface(self, *args, **kwargs))

    def scatter_with_defaults(self, *args, **kwargs):
        kwargs.setdefault("axlim_clip", False)
        return apply_artist_clip_defaults(original_scatter(self, *args, **kwargs))

    def add_collection3d_with_defaults(self, col, *args, **kwargs):
        kwargs.setdefault("axlim_clip", False)
        return apply_artist_clip_defaults(original_add_collection3d(self, col, *args, **kwargs))

    Axes3D.plot = plot_with_defaults
    Axes3D.plot_surface = plot_surface_with_defaults
    Axes3D.scatter = scatter_with_defaults
    Axes3D.add_collection3d = add_collection3d_with_defaults
    Axes3D._mathworks_3d_defaults_applied = True


_apply_matplotlib_3d_defaults()


@dataclass(frozen=True)
class ColorProfile:
    name: str
    colormap: object
    edge_color: str
    edge_alpha: float
    wireframe_color: str | None
    wireframe_alpha: float
    shading_floor: float
    shading_gain: float


@dataclass(frozen=True)
class RuntimeSettings:
    waveform_signed: bool
    surface_profile: ColorProfile


@dataclass(frozen=True)
class ChromaPalette:
    name: str
    colors: np.ndarray
    edge_color: str = "#d6e6ff"
    edge_alpha: float = 0.055
    row_wire_color: str = "#d6e6ff"
    row_wire_alpha: float = 0.105
    column_wire_color: str = "#72809a"
    column_wire_alpha: float = 0.050


@dataclass(frozen=True)
class AudioData:
    samples: np.ndarray
    sample_rate: int
    duration_seconds: float


@dataclass(frozen=True)
class SurfaceData:
    surface: np.ndarray
    position_surface: np.ndarray
    x_grid: np.ndarray
    y_grid: np.ndarray
    z_grid: np.ndarray
    x_limits: tuple[float, float]
    y_limits: tuple[float, float]
    z_limits: tuple[float, float]
    labels: tuple[str, str, str]
    height_scale: float


@dataclass(frozen=True)
class RenderSpec:
    figure_width: float = FIGURE_WIDTH
    figure_height: float = FIGURE_HEIGHT
    view_elevation: float = VIEW_ELEVATION
    view_azimuth: float = VIEW_AZIMUTH
    box_aspect: tuple[float, float, float] = BOX_ASPECT
    zoom: float = VIEW_ZOOM
    tick_label_size: float = TICK_LABEL_SIZE
    tick_padding: float = TICK_PADDING
    axis_label_size: float = AXIS_LABEL_SIZE
    x_tick_count: int = X_TICK_COUNT
    y_tick_count: int = Y_TICK_COUNT
    z_tick_count: int = Z_TICK_COUNT
    transparent_background: bool = False


@dataclass(frozen=True)
class ScribbleSpec:
    lift: float
    threshold: float = 0.04
    glow_color: str = "#8a8a8a"
    ridge_color: str = "#f4f4f4"


@dataclass(frozen=True)
class CurtainSpec:
    top_lift: float
    z_min: float
    z_max: float
    z_line: float


def create_gnuplot_pm3d_colormap():
    return LinearSegmentedColormap.from_list(
        "pm3d_black",
        [
            (0.00, "#180045"),
            (0.20, "#2430d8"),
            (0.42, "#008fe8"),
            (0.62, "#00b84d"),
            (0.80, "#e0b000"),
            (1.00, "#ff2200"),
        ],
    )


def create_dreamcast_cyan_colormap():
    return LinearSegmentedColormap.from_list(
        "dreamcast_cyan",
        [
            (0.00, "#02060b"),
            (0.10, "#061322"),
            (0.28, "#0a2d4d"),
            (0.50, "#0e5f83"),
            (0.70, "#17a2bd"),
            (0.88, "#65e7e2"),
            (1.00, "#f0ffff"),
        ],
    )


MATLAB_JET = ColorProfile(
    name="matlab_jet",
    colormap=plt.get_cmap("jet"),
    edge_color="#d6e6ff",
    edge_alpha=0.11,
    wireframe_color="#d6e6ff",
    wireframe_alpha=0.10,
    shading_floor=0.86,
    shading_gain=0.18,
)

GNUPLOT_PM3D = ColorProfile(
    name="gnuplot_pm3d",
    colormap=create_gnuplot_pm3d_colormap(),
    edge_color="#a8b7cf",
    edge_alpha=0.07,
    wireframe_color=None,
    wireframe_alpha=0.00,
    shading_floor=0.90,
    shading_gain=0.12,
)

DREAMCAST_CYAN = ColorProfile(
    name="dreamcast_cyan",
    colormap=create_dreamcast_cyan_colormap(),
    edge_color="#58ddff",
    edge_alpha=0.10,
    wireframe_color="#ff00ff",
    wireframe_alpha=0.20,
    shading_floor=0.74,
    shading_gain=0.28,
)

ALL_PROFILES = [MATLAB_JET, GNUPLOT_PM3D, DREAMCAST_CYAN]
PROFILE_BY_NAME = {profile.name: profile for profile in ALL_PROFILES}


def resolve_color_profile(name: str) -> ColorProfile:
    try:
        return PROFILE_BY_NAME[name]
    except KeyError as exc:
        valid_names = ", ".join(sorted(PROFILE_BY_NAME))
        raise ValueError(f"unknown color profile {name!r}; expected one of: {valid_names}") from exc


DEFAULT_SURFACE_PROFILE_NAME = os.environ.get("SURFACE_PROFILE", MATLAB_JET.name).strip() or MATLAB_JET.name
if DEFAULT_SURFACE_PROFILE_NAME not in PROFILE_BY_NAME:
    DEFAULT_SURFACE_PROFILE_NAME = MATLAB_JET.name
DEFAULT_SURFACE_PROFILE = resolve_color_profile(DEFAULT_SURFACE_PROFILE_NAME)
DEFAULT_WAVEFORM_RUNTIME = RuntimeSettings(
    waveform_signed=False,
    surface_profile=DEFAULT_SURFACE_PROFILE,
)
SIGNED_WAVEFORM_RUNTIME = RuntimeSettings(
    waveform_signed=True,
    surface_profile=DEFAULT_SURFACE_PROFILE,
)
SELECTED_COLOR_PROFILE = DEFAULT_SURFACE_PROFILE


def hex_palette_to_rgb01(values: list[str]) -> np.ndarray:
    return np.array([to_rgba(v)[:3] for v in values], dtype=np.float32)


RAYLIB_MAGENTA = "#ff00ff"
RAYLIB_BLUE = "#0079f1"
RAYLIB_RED = "#e62937"
RAYLIB_YELLOW = "#fdf900"
NEUTRAL_WIREFRAME_COLOR = "#72809a"

WAVEFORM_HILBERT_BASE_WIRE_COLOR = NEUTRAL_WIREFRAME_COLOR
WAVEFORM_HILBERT_BASE_WIRE_ALPHA = 0.24

WAVEFORM_ONSET_SCRIBBLE_GLOW_COLOR = "#8a8a8a"
WAVEFORM_ONSET_SCRIBBLE_RIDGE_COLOR = "#f4f4f4"

WAVEFORM_ONSET_CURTAIN_FILL_COLOR = "#777777"
WAVEFORM_ONSET_CURTAIN_RIDGE_COLOR = "#f2f2f2"
WAVEFORM_ONSET_CURTAIN_FLOOR_COLOR = "#d8d8d8"

WAVEFORM_BRIGHTNESS_WASH_DARK = np.array([0.015, 0.015, 0.015], dtype=np.float32)
WAVEFORM_BRIGHTNESS_WASH_CYAN = np.array([0.54, 0.54, 0.54], dtype=np.float32)
WAVEFORM_BRIGHTNESS_WASH_WHITE = np.array([1.00, 1.00, 1.00], dtype=np.float32)

WAVEFORM_RMS_COLORMAP = LinearSegmentedColormap.from_list(
    "waveform_rms_recovered_exact_raylib_endpoints",
    [
        (0.00, "#000000"),
        (0.14, "#05020a"),
        (0.26, "#160032"),
        (0.35, RAYLIB_MAGENTA),
        (0.56, RAYLIB_BLUE),
        (0.78, RAYLIB_RED),
        (1.00, RAYLIB_YELLOW),
    ],
)
WAVEFORM_RMS_ROW_WIRE_COLOR = RAYLIB_MAGENTA
WAVEFORM_RMS_COLUMN_WIRE_COLOR = NEUTRAL_WIREFRAME_COLOR
WAVEFORM_RMS_EDGE_COLOR = TEXT_COLOR
WAVEFORM_RMS_EDGE_ALPHA = 0.050


def build_runtime_vertex_rgba(row_count: int, column_count: int) -> np.ndarray:
    magenta = np.array(to_rgba(RAYLIB_MAGENTA), dtype=np.float32)
    blue = np.array(to_rgba(RAYLIB_BLUE), dtype=np.float32)
    red = np.array(to_rgba(RAYLIB_RED), dtype=np.float32)
    yellow = np.array(to_rgba(RAYLIB_YELLOW), dtype=np.float32)
    u = np.linspace(0.0, 1.0, column_count, dtype=np.float32)[None, :]
    v = np.linspace(0.0, 1.0, row_count, dtype=np.float32)[:, None]
    rgba = ((1.0 - u) * (1.0 - v))[..., None] * magenta + (u * (1.0 - v))[..., None] * blue + ((1.0 - u) * v)[..., None] * red + (u * v)[..., None] * yellow
    rgba[..., 3] = 1.0
    return rgba.astype(np.float32)


WUXING_CHROMA_HEX = [
    "#fff799",
    "#ffb61e",
    "#fffbf0",
    "#f1f2f4",
    "#1ba784",
    "#3b818c",
    "#0f59a4",
    "#be002f",
    "#c02c38",
    "#3d3b4f",
    "#806d9e",
    "#ca6924",
]

NEWTONIAN_CHROMA_HEX = [
    "#ff3b30",
    "#ff5f1f",
    "#ff7a00",
    "#ff9500",
    "#ffd60a",
    "#b7d300",
    "#34c759",
    "#00c7be",
    "#0a84ff",
    "#3f51ff",
    "#6e45e2",
    "#bf5af2",
]

WUXING_CHROMA = ChromaPalette(
    name="wuxing",
    colors=hex_palette_to_rgb01(WUXING_CHROMA_HEX),
)

NEWTONIAN_CHROMA = ChromaPalette(
    name="newtonian",
    colors=hex_palette_to_rgb01(NEWTONIAN_CHROMA_HEX),
)

ALL_CHROMA_PALETTES = [WUXING_CHROMA, NEWTONIAN_CHROMA]
SELECTED_CHROMA_PALETTE = WUXING_CHROMA


def load_wav_file(filepath: Path) -> tuple[np.ndarray, int]:
    with wave.open(str(filepath), "rb") as wav_file:
        channel_count = wav_file.getnchannels()
        sample_rate = wav_file.getframerate()
        frame_count = wav_file.getnframes()
        raw_data = np.frombuffer(wav_file.readframes(frame_count), dtype="<i2").astype(np.float32)
    if channel_count > 1:
        raw_data = raw_data.reshape(-1, channel_count).mean(1)
    return np.clip(raw_data, -32767, 32767) / 32767.0, sample_rate


def compute_bin_edges(total_length: int, bin_count: int) -> np.ndarray:
    return np.round(np.linspace(0, total_length, bin_count + 1)).astype(int)


def aggregate_along_axis(data: np.ndarray, target_size: int, axis: int, mean_weight: float = 0.67) -> np.ndarray:
    if data.shape[axis] <= target_size:
        return data.copy()
    edges = compute_bin_edges(data.shape[axis], target_size)
    if axis == 0:
        output = np.zeros((target_size, data.shape[1]), np.float32)
        for i, (start, end) in enumerate(zip(edges[:-1], edges[1:])):
            segment = data[start : max(start + 1, end), :]
            output[i] = mean_weight * segment.mean(0) + (1 - mean_weight) * segment.max(0)
    else:
        output = np.zeros((data.shape[0], target_size), np.float32)
        for i, (start, end) in enumerate(zip(edges[:-1], edges[1:])):
            segment = data[:, start : max(start + 1, end)]
            output[:, i] = mean_weight * segment.mean(1) + (1 - mean_weight) * segment.max(1)
    return output


def normalize_surface(data: np.ndarray, percentile: float = 99.6, gamma: float = 0.78) -> np.ndarray:
    ceiling = max(float(np.percentile(data, percentile)), 1e-9)
    return np.power(np.clip(data, 0, ceiling) / ceiling, gamma).astype(np.float32)


def smooth_surface(data: np.ndarray, sigma: tuple[float, float] = (0.70, 0.55)) -> np.ndarray:
    try:
        from scipy.ndimage import gaussian_filter

        return gaussian_filter(data, sigma=sigma, mode="nearest").astype(np.float32)
    except Exception:
        return data.astype(np.float32, copy=True)


def audio_frame_starts(samples: np.ndarray) -> np.ndarray:
    return np.arange(0, len(samples), HOP_SIZE)


def iter_audio_frames(samples: np.ndarray):
    for start in audio_frame_starts(samples):
        frame = np.zeros(FFT_WINDOW_SIZE, np.float32)
        available_samples = max(0, min(FFT_WINDOW_SIZE, len(samples) - start))
        if available_samples:
            frame[:available_samples] = samples[start : start + available_samples]
        yield frame


def interp_rows(data: np.ndarray, target_points: int, *, grid_dtype=np.float32) -> np.ndarray:
    linspace_kwargs = {} if grid_dtype is None else {"dtype": grid_dtype}
    source_x = np.linspace(0.0, 1.0, data.shape[1], **linspace_kwargs)
    target_x = np.linspace(0.0, 1.0, target_points, **linspace_kwargs)
    return np.vstack([np.interp(target_x, source_x, row) for row in data]).astype(np.float32)


def project_waveform_history(
    frame_history: np.ndarray,
    *,
    target_lanes: int = TOTAL_LANES,
    target_points: int = WAVEFORM_POINTS,
    mean_weight: float = 0.72,
    smooth_sigma: tuple[float, float] = (0.70, 0.55),
) -> np.ndarray:
    aggregated = aggregate_along_axis(frame_history, target_lanes, 0, mean_weight=mean_weight)
    return smooth_surface(interp_rows(aggregated, target_points), sigma=smooth_sigma)


def compute_waveform_amplitude_history(samples: np.ndarray, points_per_frame: int = RUNTIME_POINTS) -> np.ndarray:
    samples_per_point = FFT_WINDOW_SIZE // points_per_frame
    amplitude_data = np.zeros((len(audio_frame_starts(samples)), points_per_frame), np.float32)
    for i, frame in enumerate(iter_audio_frames(samples)):
        for j in range(points_per_frame):
            chunk = frame[j * samples_per_point : min((j + 1) * samples_per_point, FFT_WINDOW_SIZE)]
            amplitude_data[i, j] = float(np.mean(np.abs(chunk)))
    return amplitude_data


def compute_waveform_lane_history(samples: np.ndarray, points_per_frame: int = RUNTIME_POINTS) -> np.ndarray:
    center_indices = ((np.arange(points_per_frame, dtype=np.int32) * (FFT_WINDOW_SIZE - 1)) // max(1, points_per_frame - 1)).astype(np.int32)
    lane_history = np.zeros((len(audio_frame_starts(samples)), points_per_frame), dtype=np.float32)

    for frame_i, frame in enumerate(iter_audio_frames(samples)):
        lane_history[frame_i] = frame[center_indices]

    return lane_history


def compute_waveform_hilbert_mask_history(samples: np.ndarray, points_per_frame: int = RUNTIME_POINTS) -> np.ndarray:
    filter_length = 65
    filter_radius = filter_length // 2
    contact_ratio_min = 0.72
    curvature_min = 0.012
    curvature_max = 0.075
    amplitude_min = 0.045
    amplitude_max = 0.20
    neighbor_contact_scale = 0.70

    filter_kernel = np.zeros(filter_length, dtype=np.float32)
    odd_offsets = np.arange(1 - filter_radius, filter_radius, 2, dtype=np.int32)
    normalized_offsets = odd_offsets.astype(np.float32) / float(filter_radius)
    kaiser_window = np.i0(8.0 * np.sqrt(np.clip(1.0 - normalized_offsets * normalized_offsets, 0.0, 1.0))) / np.i0(8.0)
    filter_kernel[odd_offsets + filter_radius] = (2.0 / (np.pi * odd_offsets.astype(np.float32))) * kaiser_window.astype(np.float32)

    center_indices = ((np.arange(points_per_frame, dtype=np.int32) * (FFT_WINDOW_SIZE - 1)) // max(1, points_per_frame - 1)).astype(np.int32)
    mask_history = np.zeros((len(audio_frame_starts(samples)), points_per_frame), dtype=np.float32)

    for frame_i, frame in enumerate(iter_audio_frames(samples)):
        analysis_mean = float(frame.mean())
        centered_frame = frame - analysis_mean
        hilbert_signal = np.convolve(centered_frame, filter_kernel, mode="same").astype(np.float32)
        waveform_samples = frame[center_indices]
        centered_samples = centered_frame[center_indices]
        hilbert_samples = hilbert_signal[center_indices]

        contact_mask = np.zeros(points_per_frame, dtype=np.float32)
        for point_i in range(points_per_frame):
            waveform_sample = float(waveform_samples[point_i])
            analytic_envelope = float(np.hypot(centered_samples[point_i], hilbert_samples[point_i]))
            if analytic_envelope <= 1.0e-9:
                continue

            envelope_reference = analysis_mean + analytic_envelope if waveform_sample >= analysis_mean else analysis_mean - analytic_envelope
            contact_ratio = 1.0 - (abs(waveform_sample - envelope_reference) / analytic_envelope)
            contact_ratio = smoothstep01((contact_ratio - contact_ratio_min) / (1.0 - contact_ratio_min))

            prev_i = max(point_i - 1, 0)
            next_i = min(point_i + 1, points_per_frame - 1)
            waveform_prev = float(waveform_samples[prev_i])
            waveform_next = float(waveform_samples[next_i])

            local_curvature = abs(waveform_prev - 2.0 * waveform_sample + waveform_next)
            curvature_mask = smoothstep01((local_curvature - curvature_min) / (curvature_max - curvature_min))

            waveform_magnitude = abs(waveform_sample - analysis_mean)
            amplitude_mask = smoothstep01((waveform_magnitude - amplitude_min) / (amplitude_max - amplitude_min))

            extrema_mask = 0.0
            if 0 < point_i < points_per_frame - 1:
                is_peak = waveform_sample > waveform_prev and waveform_sample >= waveform_next
                is_trough = waveform_sample < waveform_prev and waveform_sample <= waveform_next
                extrema_mask = 1.0 if (is_peak or is_trough) else 0.0

            contact_mask[point_i] = np.clip((contact_ratio**1.75) * curvature_mask * amplitude_mask * extrema_mask, 0.0, 1.0)

        for point_i in range(points_per_frame):
            neighbor_mask = contact_mask[point_i]
            if point_i > 0:
                neighbor_mask = max(neighbor_mask, float(contact_mask[point_i - 1]) * neighbor_contact_scale)
            if point_i < points_per_frame - 1:
                neighbor_mask = max(neighbor_mask, float(contact_mask[point_i + 1]) * neighbor_contact_scale)
            neighbor_mask = smoothstep01((neighbor_mask - 0.08) / (0.82 - 0.08))
            mask_history[frame_i, point_i] = float(np.power(neighbor_mask, 0.65))

    return mask_history


def compute_waveform_onset_history(samples: np.ndarray, points_per_frame: int = RUNTIME_POINTS) -> np.ndarray:
    lane_history = compute_waveform_lane_history(samples, points_per_frame=points_per_frame)
    frame_count = lane_history.shape[0]
    onset_gate_history = np.zeros(frame_count, dtype=np.float32)
    onset_gate = 0.0
    adaptive_waveform_onset_power_mean = 0.0
    adaptive_waveform_onset_power_deviation = 0.0
    adaptive_waveform_onset_power_ready = False

    for frame_i in range(1, frame_count):
        lane_delta = np.maximum(np.abs(lane_history[frame_i]) - np.abs(lane_history[frame_i - 1]), 0.0)
        onset_delta_power = float(np.mean(lane_delta * lane_delta))

        if not adaptive_waveform_onset_power_ready:
            adaptive_waveform_onset_power_mean = onset_delta_power
            adaptive_waveform_onset_power_deviation = onset_delta_power
            adaptive_waveform_onset_power_ready = True
        else:
            adaptive_waveform_onset_power_delta = abs(onset_delta_power - adaptive_waveform_onset_power_mean)
            adaptive_waveform_onset_power_mean = (
                1.0 - GLOBAL_ADAPTIVE_GATE_SMOOTHING_FACTOR
            ) * adaptive_waveform_onset_power_mean + GLOBAL_ADAPTIVE_GATE_SMOOTHING_FACTOR * onset_delta_power
            adaptive_waveform_onset_power_deviation = (
                1.0 - GLOBAL_ADAPTIVE_GATE_SMOOTHING_FACTOR
            ) * adaptive_waveform_onset_power_deviation + GLOBAL_ADAPTIVE_GATE_SMOOTHING_FACTOR * adaptive_waveform_onset_power_delta

        adaptive_waveform_onset_power_floor = (
            adaptive_waveform_onset_power_mean - adaptive_waveform_onset_power_deviation * GLOBAL_ADAPTIVE_GATE_FLOOR_DEVIATION_SCALE
        )
        adaptive_waveform_onset_power_ceiling = (
            adaptive_waveform_onset_power_mean + adaptive_waveform_onset_power_deviation * GLOBAL_ADAPTIVE_GATE_CEILING_DEVIATION_SCALE
        )
        onset_gate_target = 0.0
        if adaptive_waveform_onset_power_ceiling > adaptive_waveform_onset_power_floor:
            onset_gate_target = float(
                np.clip(
                    (onset_delta_power - adaptive_waveform_onset_power_floor) / (adaptive_waveform_onset_power_ceiling - adaptive_waveform_onset_power_floor),
                    0.0,
                    1.0,
                )
            )

        if onset_gate_target > onset_gate:
            onset_gate = onset_gate_target
        else:
            onset_gate = float((1.0 - ONSET_DECAY_RATE) * onset_gate + ONSET_DECAY_RATE * onset_gate_target)

        onset_gate_history[frame_i] = float(np.clip(onset_gate, 0.0, 1.0))

    return onset_gate_history


def compute_fft_history(samples: np.ndarray) -> tuple[np.ndarray, np.ndarray]:
    frame_starts = audio_frame_starts(samples)
    window = np.blackman(FFT_WINDOW_SIZE).astype(np.float32)
    raw_magnitudes = np.zeros((len(frame_starts), FFT_BINS), np.float32)
    smoothed_levels = np.zeros((len(frame_starts), FFT_BINS), np.float32)
    smooth_accumulator = np.zeros(FFT_BINS, np.float32)
    inverse_decibel_range = 1.0 / (DECIBEL_MAXIMUM - DECIBEL_MINIMUM)
    for i, frame in enumerate(iter_audio_frames(samples)):
        magnitude = np.abs(np.fft.fft(frame * window)[:FFT_BINS]) / FFT_WINDOW_SIZE
        raw_magnitudes[i] = magnitude
        smooth_accumulator = SMOOTHING_FACTOR * smooth_accumulator + (1 - SMOOTHING_FACTOR) * magnitude
        decibel_values = 20 * np.log10(np.maximum(smooth_accumulator, 1e-12))
        smoothed_levels[i] = np.clip((decibel_values - DECIBEL_MINIMUM) * inverse_decibel_range, 0, 1)
    return raw_magnitudes, smoothed_levels


def compute_waveform_surface(samples: np.ndarray) -> np.ndarray:
    amplitude_data = compute_waveform_amplitude_history(samples)
    amplitude_data = project_waveform_history(amplitude_data, mean_weight=0.72, smooth_sigma=(0.70, 0.55))
    return normalize_surface(amplitude_data, 99.7, 0.72)


def compute_signed_waveform_surface(
    samples: np.ndarray,
    *,
    target_lanes: int = TOTAL_LANES,
    target_points: int = WAVEFORM_POINTS,
) -> np.ndarray:
    lane_history = compute_waveform_lane_history(samples, points_per_frame=RUNTIME_POINTS)
    return project_waveform_history(
        lane_history,
        target_lanes=target_lanes,
        target_points=target_points,
        mean_weight=1.0,
        smooth_sigma=(0.0, 0.0),
    )


def compute_waveform_position_surface(
    samples: np.ndarray,
    *,
    unsigned_surface: np.ndarray | None = None,
    target_lanes: int = TOTAL_LANES,
    target_points: int = WAVEFORM_POINTS,
    signed: bool = False,
) -> np.ndarray:
    if unsigned_surface is not None:
        target_lanes, target_points = unsigned_surface.shape
    if signed:
        return compute_signed_waveform_surface(
            samples,
            target_lanes=target_lanes,
            target_points=target_points,
        )
    if unsigned_surface is not None:
        return unsigned_surface
    if target_points == WAVEFORM_POINTS and target_lanes == TOTAL_LANES:
        return compute_waveform_surface(samples)
    amplitude_data = compute_waveform_amplitude_history(samples)
    amplitude_data = project_waveform_history(
        amplitude_data,
        target_lanes=target_lanes,
        target_points=target_points,
        mean_weight=0.72,
        smooth_sigma=(0.70, 0.55),
    )
    return normalize_surface(amplitude_data, 99.7, 0.72)


def get_waveform_z_limits(extra_top: float = 0.0, *, signed: bool = False) -> tuple[float, float]:
    z_min = -WAVEFORM_Z_MAX if signed else WAVEFORM_Z_MIN
    return z_min, WAVEFORM_Z_MAX + extra_top


def get_waveform_output_path(output_path: Path, *, signed: bool = False) -> Path:
    if not signed:
        return output_path
    return output_path.with_name(f"{output_path.stem}_signed{output_path.suffix}")


def compute_fft_surface(smoothed_levels: np.ndarray, points: int = FFT_POINTS) -> np.ndarray:
    return normalize_surface(
        aggregate_along_axis(aggregate_along_axis(smoothed_levels[:, 1:], points, 1), TOTAL_LANES, 0),
        99.6,
        0.78,
    )


def compute_band_frame(values: np.ndarray, bounds: list) -> np.ndarray:
    return np.array([values[start : end + 1].mean() for start, end in bounds], np.float32)


def compute_frequency_band_histories(raw_magnitudes: np.ndarray, smoothed_levels: np.ndarray) -> tuple[np.ndarray, np.ndarray, np.ndarray]:
    low_band = np.zeros((raw_magnitudes.shape[0], LOW_BAND_POINT_COUNT), np.float32)
    mid_band = np.zeros((raw_magnitudes.shape[0], MID_BAND_POINT_COUNT), np.float32)
    high_band = np.zeros((raw_magnitudes.shape[0], HIGH_BAND_POINT_COUNT), np.float32)
    mid_peak = high_peak = 1e-12
    for i in range(raw_magnitudes.shape[0]):
        low_band[i] = compute_band_frame(smoothed_levels[i], LOW_BAND_BOUNDS)
        mid_linear = compute_band_frame(raw_magnitudes[i], MID_BAND_BOUNDS)
        high_linear = compute_band_frame(raw_magnitudes[i], HIGH_BAND_BOUNDS)
        mid_peak = max(mid_peak * 0.985, float(mid_linear.max()), 1e-12)
        high_peak = max(high_peak * 0.985, float(high_linear.max()), 1e-12)
        mid_band[i] = (
            np.clip(
                1 + (np.log(np.maximum(mid_linear / mid_peak, 1e-12)) * DECIBEL_SCALE_FACTOR) / 40.0,
                0,
                1,
            )
            * 3.0
        )
        high_band[i] = np.clip(
            1 + (np.log(np.maximum(high_linear / high_peak, 1e-12)) * DECIBEL_SCALE_FACTOR) / 40.0,
            0,
            1,
        )
    return low_band, mid_band, high_band


def compute_frequency_bands(raw_magnitudes: np.ndarray, smoothed_levels: np.ndarray) -> tuple[np.ndarray, np.ndarray, np.ndarray]:
    low_band, mid_band, high_band = compute_frequency_band_histories(raw_magnitudes, smoothed_levels)
    return (
        aggregate_along_axis(low_band, TOTAL_LANES, 0),
        aggregate_along_axis(mid_band, TOTAL_LANES, 0),
        aggregate_along_axis(high_band, TOTAL_LANES, 0),
    )


def compute_fft_onset_history(raw_magnitudes: np.ndarray, smoothed_levels: np.ndarray, use_raw: bool = True) -> np.ndarray:
    raw_db = np.log(np.maximum(raw_magnitudes, 1.0e-12)) * DECIBEL_SCALE_FACTOR
    level_history = raw_db if use_raw else smoothed_levels
    return compute_flux_onset_gate_history(level_history, envelope_radius=ONSET_ENVELOPE_RADIUS)


def compute_low_band_onset_history(raw_magnitudes: np.ndarray, smoothed_levels: np.ndarray) -> np.ndarray:
    low_band, _, _ = compute_frequency_band_histories(raw_magnitudes, smoothed_levels)
    return compute_flux_onset_gate_history(low_band, envelope_radius=1)


def compute_flux_onset_gate_history(level_history: np.ndarray, envelope_radius: int = ONSET_ENVELOPE_RADIUS) -> np.ndarray:
    level_history = np.asarray(level_history, dtype=np.float32)
    frame_count = level_history.shape[0]
    point_count = level_history.shape[1]
    onset_gate_history = np.zeros(frame_count, dtype=np.float32)
    onset_gate = 0.0
    adaptive_onset_mean = 0.0
    adaptive_onset_deviation = 0.0

    for frame_i in range(frame_count):
        if frame_i <= ONSET_DELAY_FRAMES:
            onset_gate_history[frame_i] = onset_gate
            continue

        cur_levels = level_history[frame_i]
        ref_levels = level_history[frame_i - ONSET_DELAY_FRAMES]
        flux_sum = 0.0

        for point_i in range(point_count):
            cur = float(cur_levels[point_i])
            ref = float(ref_levels[point_i])
            j_min = max(point_i - envelope_radius, 0)
            j_max = min(point_i + envelope_radius, point_count - 1)
            for ref_i in range(j_min, j_max + 1):
                ref = max(ref, float(ref_levels[ref_i]))
            flux_sum += max(cur - ref, 0.0)

        onset_flux = flux_sum / float(point_count)
        adaptive_onset_delta = abs(onset_flux - adaptive_onset_mean)
        adaptive_onset_mean = (1.0 - ADAPTIVE_ONSET_RATE) * adaptive_onset_mean + ADAPTIVE_ONSET_RATE * onset_flux
        adaptive_onset_deviation = (1.0 - ADAPTIVE_ONSET_RATE) * adaptive_onset_deviation + ADAPTIVE_ONSET_RATE * adaptive_onset_delta
        adaptive_onset_floor = adaptive_onset_mean + adaptive_onset_deviation * ADAPTIVE_ONSET_GATE_DEVIATION_SCALE
        adaptive_onset_range = adaptive_onset_deviation * ADAPTIVE_ONSET_RANGE_DEVIATION_SCALE

        onset_gate_target = 0.0
        if adaptive_onset_range > 0.0:
            onset_gate_target = np.clip((onset_flux - adaptive_onset_floor) / adaptive_onset_range, 0.0, 1.0)

        if onset_gate_target > onset_gate:
            onset_gate = float(onset_gate_target)
        else:
            onset_gate = float((1.0 - ONSET_DECAY_RATE) * onset_gate + ONSET_DECAY_RATE * onset_gate_target)
        onset_gate_history[frame_i] = np.clip(onset_gate, 0.0, 1.0)

    return onset_gate_history


def resample_onset_gate_rows(
    row_times: np.ndarray,
    frame_times: np.ndarray,
    onset_gate_history: np.ndarray,
) -> np.ndarray:
    if onset_gate_history.size == 0:
        return np.zeros_like(row_times, dtype=np.float32)

    unique_times, unique_indices = np.unique(frame_times.astype(np.float32), return_index=True)
    unique_gate = onset_gate_history[unique_indices].astype(np.float32)
    return np.interp(
        row_times.astype(np.float32),
        unique_times,
        unique_gate,
        left=float(unique_gate[0]),
        right=float(unique_gate[-1]),
    ).astype(np.float32)


def extract_visible_onset_pulses(
    onset_gate: np.ndarray,
    *,
    min_gate: float = VISIBLE_ONSET_MIN_GATE,
    merge_radius_frames: int = VISIBLE_ONSET_MERGE_FRAMES,
) -> tuple[np.ndarray, np.ndarray]:
    onset_gate = np.asarray(onset_gate, dtype=np.float32).reshape(-1)
    if onset_gate.size < 3:
        return np.array([], dtype=np.int32), np.array([], dtype=np.float32)

    peaks: list[tuple[float, int]] = []
    for frame_i in range(1, onset_gate.size - 1):
        gate_peak = float(onset_gate[frame_i])
        if gate_peak < min_gate:
            continue
        if gate_peak >= float(onset_gate[frame_i - 1]) and gate_peak >= float(onset_gate[frame_i + 1]):
            peaks.append((gate_peak, frame_i))

    if not peaks:
        return np.array([], dtype=np.int32), np.array([], dtype=np.float32)

    selected: list[tuple[int, float]] = []
    active_frame: int | None = None
    active_peak: float | None = None

    for gate_peak, frame_i in peaks:
        if active_frame is None or active_peak is None:
            active_frame = frame_i
            active_peak = gate_peak
            continue

        if frame_i - active_frame <= merge_radius_frames:
            if gate_peak > active_peak:
                active_frame = frame_i
                active_peak = gate_peak
        else:
            selected.append((active_frame, active_peak))
            active_frame = frame_i
            active_peak = gate_peak

    if active_frame is not None and active_peak is not None:
        selected.append((active_frame, active_peak))

    frames = np.array([frame_i for frame_i, _ in selected], dtype=np.int32)
    strengths = np.array([gate_peak for _, gate_peak in selected], dtype=np.float32)
    return frames, strengths


def extract_visible_waveform_onset_pulses(
    onset_gate_history: np.ndarray,
    *,
    min_gate: float = VISIBLE_ONSET_MIN_GATE,
    merge_frames: int = VISIBLE_ONSET_MERGE_FRAMES,
) -> tuple[np.ndarray, np.ndarray, np.ndarray]:
    frames, gate_peaks = extract_visible_onset_pulses(
        onset_gate_history,
        min_gate=min_gate,
        merge_radius_frames=merge_frames,
    )
    diffuse_peaks = (LIGHT0_DIFFUSE_MAX * gate_peaks).astype(np.float32)
    return frames, gate_peaks, diffuse_peaks


def limit_onset_pulses_for_display(
    frames: np.ndarray,
    gate_peaks: np.ndarray,
    diffuse_peaks: np.ndarray,
    *,
    max_events: int = CURTAIN_MAX_EVENTS_VISIBLE,
) -> tuple[np.ndarray, np.ndarray, np.ndarray]:
    if max_events <= 0 or frames.size <= max_events:
        return frames, gate_peaks, diffuse_peaks

    strongest = np.argsort(gate_peaks)[-max_events:]
    strongest = strongest[np.argsort(frames[strongest])]
    return frames[strongest], gate_peaks[strongest], diffuse_peaks[strongest]


def compute_onset_event_strengths(onset_history: np.ndarray) -> np.ndarray:
    baseline_kernel = ONSET_EVENT_BASELINE_KERNEL / np.sum(ONSET_EVENT_BASELINE_KERNEL)
    baseline = np.convolve(onset_history, baseline_kernel, mode="same")
    prominence = np.clip(onset_history - baseline * 0.99, 0.0, None)
    floor = max(float(np.percentile(prominence, 95.0)), 0.05 * float(prominence.max()))
    ceiling = max(float(np.percentile(prominence, 99.7)), floor + 1.0e-6)
    normalized = np.clip((prominence - floor) / (ceiling - floor), 0.0, 1.0)
    return np.power(normalized, 0.68).astype(np.float32)


def select_onset_events(
    onset_history: np.ndarray,
    *,
    refractory_frames: int = ONSET_EVENT_REFRACTORY_FRAMES,
    threshold_percentile: float = 84.0,
) -> tuple[np.ndarray, np.ndarray]:
    event_strengths = compute_onset_event_strengths(onset_history)
    if event_strengths.size < 3:
        return np.array([], dtype=np.int32), np.array([], dtype=np.float32)

    threshold = max(0.08, float(np.percentile(event_strengths, threshold_percentile)))
    peaks: list[tuple[float, int]] = []

    for row_i in range(2, event_strengths.size - 2):
        strength = float(event_strengths[row_i])
        if strength < threshold:
            continue
        if strength >= float(event_strengths[row_i - 1]) and strength >= float(event_strengths[row_i + 1]):
            peaks.append((strength, row_i))

    if not peaks:
        peak_row = int(np.argmax(event_strengths))
        if float(event_strengths[peak_row]) > 0.0:
            peaks.append((float(event_strengths[peak_row]), peak_row))

    peaks.sort(key=lambda item: item[1])
    selected: list[int] = []
    selected_strengths: list[float] = []
    active_row = peaks[0][1]
    active_strength = peaks[0][0]

    for strength, row_i in peaks[1:]:
        if row_i - active_row <= refractory_frames:
            if strength > active_strength:
                active_row = row_i
                active_strength = strength
            continue
        selected.append(active_row)
        selected_strengths.append(active_strength)
        active_row = row_i
        active_strength = strength

    selected.append(active_row)
    selected_strengths.append(active_strength)

    raw_strengths = np.array(selected_strengths, dtype=np.float32)
    strength_floor = float(raw_strengths.min())
    strength_ceiling = max(float(raw_strengths.max()), strength_floor + 1.0e-6)
    display_strengths = 0.20 + 0.80 * np.power(
        np.clip((raw_strengths - strength_floor) / (strength_ceiling - strength_floor), 0.0, 1.0),
        0.78,
    )
    return np.array(selected, dtype=np.int32), display_strengths.astype(np.float32)


def compute_event_times(frame_count: int, duration_seconds: float) -> np.ndarray:
    frame_step_seconds = HOP_SIZE / float(SAMPLE_RATE)
    return np.minimum(np.arange(frame_count, dtype=np.float32) * frame_step_seconds, duration_seconds)


def map_event_frame_to_surface_row(frame_i: int, frame_count: int, row_count: int) -> int:
    return int(np.clip(round((row_count - 1) * frame_i / max(1, frame_count - 1)), 0, row_count - 1))


def compute_onset_wash_rows(
    row_times: np.ndarray,
    frame_count: int,
    duration_seconds: float,
    event_frames: np.ndarray,
    event_strengths: np.ndarray,
) -> np.ndarray:
    wash_rows = np.zeros_like(row_times, dtype=np.float32)
    if event_frames.size == 0:
        return wash_rows

    event_times = compute_event_times(frame_count, duration_seconds)
    sigma = max(float(row_times[1] - row_times[0]) * 0.72, float(HOP_SIZE) / float(SAMPLE_RATE) * 0.34)

    for frame_i, strength in zip(event_frames, event_strengths, strict=False):
        y_center = float(event_times[frame_i])
        wash_rows += strength * np.exp(-0.5 * np.square((row_times - y_center) / sigma))

    wash_peak = max(float(np.percentile(wash_rows, 99.0)), 1.0e-6)
    return np.clip(np.power(wash_rows / wash_peak, 0.92), 0.0, 1.0).astype(np.float32)


def compute_chroma_filter_bank(bin_min: int = 1, bin_max: int | None = None) -> np.ndarray:
    if bin_max is None:
        bin_max = FFT_BINS - 1
    bin_indices = np.arange(bin_min, bin_max + 1, dtype=np.int32)
    frequencies = (bin_indices.astype(np.float32) * float(SAMPLE_RATE)) / float(FFT_WINDOW_SIZE)
    octave = np.log(frequencies / CHROMA_A0_HZ) * CHROMA_INVERSE_LN_2
    chroma_pos = 12.0 * octave

    next_indices = np.minimum(bin_indices + 1, FFT_BINS - 1)
    next_frequencies = (next_indices.astype(np.float32) * float(SAMPLE_RATE)) / float(FFT_WINDOW_SIZE)
    next_octave = np.log(next_frequencies / CHROMA_A0_HZ) * CHROMA_INVERSE_LN_2
    chroma_width = np.maximum(12.0 * next_octave - chroma_pos, 1.0)
    octave_weight = np.exp(-0.5 * ((octave - 5.0) / 2.0) ** 2).astype(np.float32)

    raw_weights = np.zeros((bin_indices.size, CHROMA_COUNT), dtype=np.float32)
    for chroma_i in range(CHROMA_COUNT):
        filter_chroma = float(chroma_i) + 3.0
        delta = np.mod(chroma_pos - filter_chroma + 6.0 + 120.0, 12.0) - 6.0
        raw_weights[:, chroma_i] = np.exp(-0.5 * ((2.0 * delta) / chroma_width) ** 2)
    l2 = np.maximum(np.sqrt(np.sum(raw_weights * raw_weights, axis=1, keepdims=True)), 1.0e-12)
    return ((raw_weights / l2) * octave_weight[:, None]).astype(np.float32)


def project_scalar_history(
    history: np.ndarray,
    *,
    target_lanes: int = TOTAL_LANES,
    mean_weight: float = 0.38,
    smooth_sigma: float = 0.75,
) -> np.ndarray:
    row_signal = np.asarray(history, dtype=np.float32).reshape(-1, 1)
    projected = aggregate_along_axis(row_signal, target_lanes, 0, mean_weight=mean_weight)[:, 0]
    if smooth_sigma > 0.0:
        projected = smooth_surface(projected[:, None], sigma=(smooth_sigma, 0.0))[:, 0]
    return np.clip(projected, 0.0, 1.0)


def compute_chroma_history(
    raw_magnitudes: np.ndarray,
    ridge_history: np.ndarray,
    bin_min: int = 1,
    bin_max: int | None = None,
) -> tuple[np.ndarray, np.ndarray]:
    if bin_max is None:
        bin_max = FFT_BINS - 1
    weights = compute_chroma_filter_bank(bin_min, bin_max)
    bin_indices = np.arange(bin_min, bin_max + 1, dtype=np.int32)
    chroma_frame = np.zeros((raw_magnitudes.shape[0], CHROMA_COUNT), dtype=np.float32)
    chroma_mask = np.zeros_like(ridge_history, dtype=np.float32)

    for frame_i in range(raw_magnitudes.shape[0]):
        power = raw_magnitudes[frame_i, bin_indices] * raw_magnitudes[frame_i, bin_indices]
        chroma_sum = power @ weights
        chroma_peak = max(float(chroma_sum.max()), 1.0e-12)
        chroma_vector = np.clip(chroma_sum / chroma_peak, 0.0, 1.0).astype(np.float32)
        chroma_frame[frame_i] = chroma_vector
        best_chroma = float(chroma_vector[int(np.argmax(chroma_vector))])
        chroma_mask[frame_i] = np.clip(ridge_history[frame_i], 0.0, 1.0) * best_chroma

    return chroma_frame, chroma_mask


def aggregate_chroma_history(
    chroma_frame_history: np.ndarray,
    chroma_mask_history: np.ndarray,
    target_lanes: int = TOTAL_LANES,
) -> tuple[np.ndarray, np.ndarray]:
    aggregated_chroma = aggregate_along_axis(chroma_frame_history, target_lanes, 0)
    aggregated_mask = aggregate_along_axis(chroma_mask_history, target_lanes, 0)
    chroma_ids = np.argmax(aggregated_chroma, axis=1).astype(np.uint8)
    return chroma_ids, np.clip(aggregated_mask, 0.0, 1.0).astype(np.float32)


def apply_chroma_palette(
    surface_data: np.ndarray,
    chroma_ids: np.ndarray,
    chroma_mask: np.ndarray,
    palette: ChromaPalette,
) -> np.ndarray:
    rgba = np.zeros((*surface_data.shape, 4), dtype=np.float32)
    shading = compute_surface_shading(surface_data, SELECTED_COLOR_PROFILE.shading_floor, SELECTED_COLOR_PROFILE.shading_gain)
    lane_mask_mean = chroma_mask.mean(axis=1)
    adaptive_mean = float(lane_mask_mean.mean())
    adaptive_deviation = float(np.mean(np.abs(lane_mask_mean - adaptive_mean)))
    gate_floor = adaptive_mean - adaptive_deviation * GLOBAL_ADAPTIVE_GATE_FLOOR_DEVIATION_SCALE
    gate_ceiling = adaptive_mean + adaptive_deviation * GLOBAL_ADAPTIVE_GATE_CEILING_DEVIATION_SCALE

    for lane in range(surface_data.shape[0]):
        chroma_index = int(chroma_ids[lane])
        base = palette.colors[chroma_index]
        lane_mask = chroma_mask[lane]
        if gate_ceiling > gate_floor:
            gate = np.clip((lane_mask - gate_floor) / (gate_ceiling - gate_floor), 0.0, 1.0)
        else:
            gate = np.zeros_like(lane_mask)
        gate = np.power(gate, 0.45)
        brightness = np.clip((0.35 + 0.65 * gate) * (0.70 + 0.30 * float(ACCIDENTAL_GAIN[chroma_index])), 0.0, 1.0)
        rgba[lane, :, :3] = np.clip(base[None, :] * brightness[:, None] * shading[lane, :, None], 0.0, 1.0)
        rgba[lane, :, 3] = lane_mask

    return rgba


def compute_surface_shading(surface_data: np.ndarray, floor: float, gain: float) -> np.ndarray:
    gradient_y, gradient_x = np.gradient(surface_data)
    normal_vectors = np.dstack((-gradient_x * 1.05, -gradient_y * 0.80, np.ones_like(surface_data) * 0.90))
    normal_vectors /= np.linalg.norm(normal_vectors, axis=2, keepdims=True) + 1e-9
    light_direction = np.array([-0.36, -0.50, 0.79], np.float32)
    light_direction /= np.linalg.norm(light_direction)
    diffuse = np.clip(np.tensordot(normal_vectors, light_direction, axes=([2], [0])), 0, 1)
    shading = (floor + gain * diffuse) * np.linspace(1.03, 0.96, surface_data.shape[0])[:, None]
    return np.clip(shading, 0.50, 1.16)


def apply_color_profile(surface_data: np.ndarray, profile: ColorProfile) -> np.ndarray:
    rgba = profile.colormap(np.clip(surface_data, 0, 1))
    shading = compute_surface_shading(surface_data, profile.shading_floor, profile.shading_gain)
    rgba[..., :3] = np.clip(rgba[..., :3] * shading[..., None], 0, 1)
    rgba[..., 3] = 1
    return rgba


def build_triangle_soup(
    x_grid: np.ndarray,
    y_grid: np.ndarray,
    z_grid: np.ndarray,
    scalar_field: np.ndarray | None = None,
) -> tuple[np.ndarray, np.ndarray | None]:
    points = np.stack((x_grid, y_grid, z_grid), axis=2).astype(np.float32)
    p00 = points[:-1, :-1].reshape(-1, 3)
    p10 = points[1:, :-1].reshape(-1, 3)
    p01 = points[:-1, 1:].reshape(-1, 3)
    p11 = points[1:, 1:].reshape(-1, 3)
    triangles = np.empty((p00.shape[0] * 2, 3, 3), dtype=np.float32)
    triangles[0::2] = np.stack((p10, p01, p00), axis=1)
    triangles[1::2] = np.stack((p10, p11, p01), axis=1)
    if scalar_field is None:
        return triangles, None
    s00 = scalar_field[:-1, :-1].reshape(-1)
    s10 = scalar_field[1:, :-1].reshape(-1)
    s01 = scalar_field[:-1, 1:].reshape(-1)
    s11 = scalar_field[1:, 1:].reshape(-1)
    scalar_values = np.empty((p00.shape[0] * 2, 3), dtype=np.float32)
    scalar_values[0::2] = np.stack((s10, s01, s00), axis=1)
    scalar_values[1::2] = np.stack((s10, s11, s01), axis=1)
    return triangles, scalar_values


def compute_triangle_normals(triangles: np.ndarray) -> np.ndarray:
    edge_a = triangles[:, 1, :] - triangles[:, 0, :]
    edge_b = triangles[:, 2, :] - triangles[:, 0, :]
    normals = np.cross(edge_a, edge_b)
    normals /= np.linalg.norm(normals, axis=1, keepdims=True) + 1.0e-9
    return normals.astype(np.float32)


def compute_triangle_flat_shading(
    triangles: np.ndarray,
    floor: float = 0.74,
    gain: float = 0.28,
) -> np.ndarray:
    normals = compute_triangle_normals(triangles)
    light_direction = np.array([-0.36, -0.50, 0.79], dtype=np.float32)
    light_direction /= np.linalg.norm(light_direction)
    diffuse = np.sum(normals * light_direction[None, :], axis=1)
    diffuse = np.abs(diffuse)
    shading = floor + gain * diffuse
    depth_fade = np.linspace(1.03, 0.96, triangles.shape[0], dtype=np.float32)
    return np.clip(shading * depth_fade, 0.50, 1.18).astype(np.float32)


def apply_axis_styling(
    axes,
    tick_label_size: float = TICK_LABEL_SIZE,
    tick_padding: float = TICK_PADDING,
    x_tick_count: int = X_TICK_COUNT,
    y_tick_count: int = Y_TICK_COUNT,
    z_tick_count: int = Z_TICK_COUNT,
    transparent_background: bool = False,
):
    if transparent_background:
        axes.set_facecolor("none")
        axes.patch.set_alpha(0.0)
        axes.patch.set_clip_on(False)
    else:
        axes.set_facecolor(BACKGROUND_COLOR)
    axes.set_proj_type(PROJECTION)
    frame_rgba = to_rgba(FRAME_COLOR)
    for axis in (axes.xaxis, axes.yaxis, axes.zaxis):
        axis.set_pane_color(to_rgba(BACKGROUND_COLOR, 0.0))
        axis._axinfo["grid"].update(color=frame_rgba, linewidth=FRAME_LINE_WIDTH)
        axis._axinfo["tick"]["inward_factor"] = AXIS3D_TICK_INWARD
        axis._axinfo["tick"]["outward_factor"] = AXIS3D_TICK_OUTWARD
        axis.line.set_color(frame_rgba)
        axis.line.set_linewidth(FRAME_LINE_WIDTH)
    axes.tick_params(
        colors=TEXT_COLOR,
        labelsize=tick_label_size,
        pad=tick_padding,
        length=TICK_LENGTH,
        width=TICK_LINEWIDTH_TICK,
        direction=TICK_DIRECTION,
    )
    axes.xaxis.set_major_locator(LinearLocator(numticks=max(2, x_tick_count)))
    axes.yaxis.set_major_locator(LinearLocator(numticks=max(2, y_tick_count)))
    axes.zaxis.set_major_locator(LinearLocator(numticks=max(2, z_tick_count)))
    for axis in (axes.xaxis, axes.yaxis, axes.zaxis):
        for label in axis.get_ticklabels():
            label.set_fontfamily(FONT_FAMILY)
            label.set_color(TEXT_COLOR)
            label.set_fontsize(tick_label_size)
    axes.title.set_visible(False)
    axes.minorticks_off()


def set_axis_labels(
    axes,
    x_label: str,
    y_label: str,
    z_label: str,
    label_size: float = AXIS_LABEL_SIZE,
    *,
    x_labelpad: float = AXIS_LABEL_PADDING_X,
    y_labelpad: float = AXIS_LABEL_PADDING_Y,
    z_labelpad: float = AXIS_LABEL_PADDING_Z,
):
    base = dict(color=TEXT_COLOR, fontsize=label_size, fontfamily=FONT_FAMILY)
    axes.set_xlabel(x_label, labelpad=x_labelpad, **base)
    axes.set_ylabel(y_label, labelpad=y_labelpad, **base)
    axes.set_zlabel(z_label, labelpad=z_labelpad, **base)


def plot3(axes, x, y, z, color, linewidth) -> None:
    axes.plot(x, y, z, color=color, linewidth=linewidth, antialiased=ANTIALIASED)


def draw_wireframe_overlay(
    axes,
    x_grid: np.ndarray,
    y_grid: np.ndarray,
    z_grid: np.ndarray,
    profile: ColorProfile,
    height_scale: float,
    row_count: int,
    column_count: int,
    row_skip: int = WIREFRAME_ROW_SKIP,
    column_skip: int = WIREFRAME_COLUMN_SKIP,
    primary_linewidth: float = WIREFRAME_PRIMARY_LINEWIDTH,
    secondary_linewidth: float = WIREFRAME_SECONDARY_LINEWIDTH,
):
    if profile.wireframe_color is None:
        return
    for i in range(0, row_count, max(1, row_count // row_skip)):
        plot3(
            axes,
            x_grid[i],
            y_grid[i],
            z_grid[i] + 0.02 * height_scale,
            to_rgba(profile.wireframe_color, profile.wireframe_alpha),
            primary_linewidth,
        )
    for j in range(0, x_grid.shape[1], max(1, x_grid.shape[1] // column_skip)):
        plot3(
            axes,
            x_grid[:, j],
            y_grid[:, j],
            z_grid[:, j] + 0.012 * height_scale,
            to_rgba(NEUTRAL_WIREFRAME_COLOR, profile.wireframe_alpha * 0.45),
            secondary_linewidth,
        )


LOW_BAND_Z_MAX = 1.18
HIGH_BAND_GLITTER_Z_LIMIT = 1.2
HIGH_BAND_GLITTER_POINT_SIZE = 3.2
HIGH_BAND_GLITTER_SUBPOINTS_PER_CELL = 64
HIGH_BAND_GLITTER_NOISE_DENSITY = 0.62
HIGH_BAND_GLITTER_RANDOM_SEED = 19971110
HIGH_BAND_GLITTER_Z_BASE_OFFSET = 0.026
HIGH_BAND_GLITTER_Z_SPREAD = 0.052
HIGH_BAND_GLITTER_Y_JITTER = 0.46
HIGH_BAND_GLITTER_X_JITTER = 0.48
HIGH_BAND_FLATNESS_BOUNDS = np.asarray(HIGH_BAND_BOUNDS, dtype=np.int32)
HIGH_BAND_ADAPTIVE_PEAK_DECAY = 0.985
HIGH_BAND_ADAPTIVE_DYNAMIC_RANGE_DB = 40.0
SPECTRAL_FLATNESS_AMIN = 1.0e-10
SPECTRAL_FLATNESS_GLITTER_SILENCE_GATE_POWER_LOW = 1.0e-9
SPECTRAL_FLATNESS_GLITTER_SILENCE_GATE_POWER_HIGH = 1.0e-9
SPECTRAL_FLATNESS_GLITTER_ATTACK_RATE = 1.0
SPECTRAL_FLATNESS_GLITTER_RELEASE_RATE = 1.0


def validate_sample_rate(sample_rate: int) -> None:
    if sample_rate != SAMPLE_RATE:
        raise ValueError(f"expected {SAMPLE_RATE} Hz, got {sample_rate}")


def load_default_audio() -> AudioData:
    samples, sample_rate = load_wav_file(INPUT_WAV_FILE)
    validate_sample_rate(sample_rate)
    return AudioData(samples=samples, sample_rate=sample_rate, duration_seconds=len(samples) / float(sample_rate))


def make_output_path(filename: str, *, signed: bool = False) -> Path:
    path = ROOT / filename
    return get_waveform_output_path(path, signed=signed)


def create_3d_figure(spec: RenderSpec = RenderSpec()):
    figure = plt.figure(
        figsize=(spec.figure_width, spec.figure_height),
        dpi=FIGURE_DPI,
        facecolor=BACKGROUND_COLOR,
    )
    axes = figure.add_subplot(111, projection="3d")
    axes.view_init(elev=spec.view_elevation, azim=spec.view_azimuth)
    axes.set_box_aspect(spec.box_aspect, zoom=spec.zoom)
    apply_axis_styling(
        axes=axes,
        tick_label_size=spec.tick_label_size,
        tick_padding=spec.tick_padding,
        x_tick_count=spec.x_tick_count,
        y_tick_count=spec.y_tick_count,
        z_tick_count=spec.z_tick_count,
        transparent_background=spec.transparent_background,
    )
    return figure, axes


def finish_figure(figure, output_image_file: Path) -> None:
    figure.subplots_adjust(left=0, right=1, top=1, bottom=0)
    figure.savefig(
        output_image_file,
        facecolor=BACKGROUND_COLOR,
        dpi=FIGURE_DPI,
        bbox_inches="tight",
        pad_inches=PADDING_INCHES,
    )
    plt.close(figure)


def create_bands_figure():
    figure = plt.figure(
        figsize=(11.0, FIGURE_HEIGHT),
        dpi=FIGURE_DPI,
        facecolor=BACKGROUND_COLOR,
    )
    gridspec = figure.add_gridspec(
        3,
        2,
        width_ratios=[0.50, 0.50],
        height_ratios=[1, 1, 1],
        left=0.00,
        right=1.00,
        top=0.94,
        bottom=0.05,
        wspace=-0.16,
        hspace=0.07,
    )
    return figure, gridspec


def finish_bands_figure(figure, output_image_file: Path) -> None:
    figure.savefig(
        output_image_file,
        facecolor=BACKGROUND_COLOR,
        dpi=FIGURE_DPI,
        bbox_inches="tight",
        pad_inches=PADDING_INCHES,
    )
    plt.close(figure)


def apply_surface_axes(axes, surface_data: SurfaceData, label_size: float = AXIS_LABEL_SIZE, **label_pad) -> None:
    axes.set_xlim(*surface_data.x_limits)
    axes.set_ylim(*surface_data.y_limits)
    axes.set_zlim(*surface_data.z_limits)
    set_axis_labels(axes, *surface_data.labels, label_size, **label_pad)


def make_surface_data(
    surface: np.ndarray,
    x_values: np.ndarray,
    duration_seconds: float,
    height_scale: float,
    z_limits: tuple[float, float],
    labels: tuple[str, str, str],
    *,
    position_surface: np.ndarray | None = None,
    y_limit: float = 60.0,
) -> SurfaceData:
    row_count, column_count = surface.shape
    y_values = np.linspace(0.0, duration_seconds, row_count, dtype=np.float32)
    x_grid, y_grid = np.meshgrid(x_values, y_values)
    if position_surface is None:
        position_surface = surface
    z_grid = position_surface * height_scale
    return SurfaceData(
        surface=surface,
        position_surface=position_surface,
        x_grid=x_grid,
        y_grid=y_grid,
        z_grid=z_grid,
        x_limits=(float(x_values[0]), float(x_values[-1])),
        y_limits=(0.0, float(y_limit)),
        z_limits=z_limits,
        labels=labels,
        height_scale=height_scale,
    )


def build_waveform_surface_data(
    samples: np.ndarray,
    duration_seconds: float,
    *,
    signed: bool = False,
    unsigned_surface: np.ndarray | None = None,
    target_points: int = WAVEFORM_POINTS,
    extra_top: float = 0.0,
) -> SurfaceData:
    if unsigned_surface is None:
        unsigned_surface = compute_waveform_surface(samples)
    position_surface = compute_waveform_position_surface(
        samples,
        unsigned_surface=unsigned_surface,
        target_points=target_points,
        signed=signed,
    )
    x_values = np.arange(unsigned_surface.shape[1], dtype=np.float32)
    return make_surface_data(
        unsigned_surface,
        x_values,
        duration_seconds,
        WAVEFORM_HEIGHT_SCALE,
        get_waveform_z_limits(extra_top=extra_top, signed=signed),
        (WAVEFORM_X_LABEL, WAVEFORM_Y_LABEL, WAVEFORM_Z_LABEL),
        position_surface=position_surface,
    )


def build_fft_surface_data(surface: np.ndarray, duration_seconds: float, *, extra_top: float = 0.0) -> SurfaceData:
    x_values = np.linspace(0.0, 10000.0, surface.shape[1], dtype=np.float32)
    return make_surface_data(
        surface,
        x_values,
        duration_seconds,
        FFT_HEIGHT_SCALE,
        (FFT_Z_MIN, FFT_Z_MAX + extra_top),
        (FFT_X_LABEL, FFT_Y_LABEL, FFT_Z_LABEL),
    )


def compute_band_center_hz(bounds) -> np.ndarray:
    bounds = np.asarray(bounds, dtype=np.float32)
    bin_hz = float(SAMPLE_RATE) / float(FFT_WINDOW_SIZE)
    return bounds.mean(axis=1) * bin_hz


def compute_low_band_center_hz() -> np.ndarray:
    return compute_band_center_hz(LOW_BAND_BOUNDS)


def compute_high_band_center_hz(bounds: np.ndarray | None = None) -> np.ndarray:
    if bounds is None:
        bounds = HIGH_BAND_BOUNDS
    return compute_band_center_hz(bounds)


def normalize_band_surface(band_data: np.ndarray) -> np.ndarray:
    return normalize_surface(band_data / max(float(np.nanmax(band_data)), 1.0e-9), 99.5, 0.78)


def build_low_band_surface_data(low_band: np.ndarray, duration_seconds: float, *, extra_top: float = 0.0) -> SurfaceData:
    return make_surface_data(
        normalize_band_surface(low_band),
        compute_low_band_center_hz(),
        duration_seconds,
        LOW_BAND_HEIGHT_SCALE,
        (0.0, LOW_BAND_Z_MAX + extra_top),
        (FFT_X_LABEL, FFT_Y_LABEL, FFT_Z_LABEL),
    )


def build_indexed_band_surface_data(
    surface: np.ndarray,
    duration_seconds: float,
    height_scale: float,
    z_limits: tuple[float, float],
    labels: tuple[str, str, str],
) -> SurfaceData:
    x_values = np.arange(surface.shape[1], dtype=np.float32)
    return make_surface_data(surface, x_values, duration_seconds, height_scale, z_limits, labels)


def render_surface(
    axes,
    surface_data: SurfaceData,
    facecolors: np.ndarray,
    *,
    linewidth: float = SURFACE_EDGE_LINEWIDTH,
    edgecolor=None,
    shade: bool = False,
) -> None:
    kwargs = dict(
        facecolors=facecolors,
        rstride=1,
        cstride=1,
        linewidth=linewidth,
        antialiased=ANTIALIASED,
        shade=shade,
    )
    if edgecolor is not None:
        kwargs["edgecolor"] = edgecolor
    axes.plot_surface(surface_data.x_grid, surface_data.y_grid, surface_data.z_grid, **kwargs)


def render_profile_surface(
    axes,
    surface_data: SurfaceData,
    profile: ColorProfile,
    *,
    linewidth: float = SURFACE_EDGE_LINEWIDTH,
    edge_alpha: float | None = None,
) -> None:
    render_surface(
        axes,
        surface_data,
        apply_color_profile(surface_data.surface, profile),
        linewidth=linewidth,
        edgecolor=to_rgba(profile.edge_color, profile.edge_alpha if edge_alpha is None else edge_alpha),
    )


def render_profile_surface_with_wireframe(
    axes,
    surface_data: SurfaceData,
    profile: ColorProfile,
    *,
    linewidth: float = SURFACE_EDGE_LINEWIDTH,
    row_skip: int = WIREFRAME_ROW_SKIP,
    column_skip: int = WIREFRAME_COLUMN_SKIP,
    primary_linewidth: float = WIREFRAME_PRIMARY_LINEWIDTH,
    secondary_linewidth: float = WIREFRAME_SECONDARY_LINEWIDTH,
    edge_alpha: float | None = None,
) -> None:
    render_profile_surface(axes, surface_data, profile, linewidth=linewidth, edge_alpha=edge_alpha)
    draw_wireframe_overlay(
        axes=axes,
        x_grid=surface_data.x_grid,
        y_grid=surface_data.y_grid,
        z_grid=surface_data.z_grid,
        profile=profile,
        height_scale=surface_data.height_scale,
        row_count=surface_data.surface.shape[0],
        column_count=surface_data.surface.shape[1],
        row_skip=row_skip,
        column_skip=column_skip,
        primary_linewidth=primary_linewidth,
        secondary_linewidth=secondary_linewidth,
    )


def compute_fft_ridge_history(smoothed_levels: np.ndarray) -> np.ndarray:
    reduced = aggregate_along_axis(smoothed_levels[:, 1:], FFT_POINTS, 1)
    return normalize_surface(reduced, 99.6, 0.78)


def draw_onset_scribbles(axes, surface_data: SurfaceData, onset_rows: np.ndarray, spec: ScribbleSpec) -> None:
    for row_i in range(surface_data.z_grid.shape[0]):
        strength = float(onset_rows[row_i])
        if strength <= spec.threshold:
            continue
        visibility = np.power((strength - spec.threshold) / (1.0 - spec.threshold), 0.82)
        plot3(
            axes,
            surface_data.x_grid[row_i],
            surface_data.y_grid[row_i],
            surface_data.z_grid[row_i] + spec.lift,
            to_rgba(spec.glow_color, 0.05 + 0.28 * visibility),
            0.40 + 2.20 * visibility,
        )
        plot3(
            axes,
            surface_data.x_grid[row_i],
            surface_data.y_grid[row_i],
            surface_data.z_grid[row_i] + spec.lift * 1.12,
            to_rgba(spec.ridge_color, 0.10 + 0.82 * visibility),
            0.08 + 0.95 * visibility,
        )


def draw_onset_curtains(axes, surface_data: SurfaceData, onset_history: np.ndarray, spec: CurtainSpec) -> None:
    event_frames, event_gate_peaks, event_diffuse_peaks = extract_visible_waveform_onset_pulses(
        onset_history,
        min_gate=CURTAIN_MIN_GATE,
        merge_frames=CURTAIN_MERGE_FRAMES,
    )
    if CURTAIN_MAX_EVENTS_VISIBLE > 0:
        event_frames, event_gate_peaks, event_diffuse_peaks = limit_onset_pulses_for_display(
            event_frames,
            event_gate_peaks,
            event_diffuse_peaks,
            max_events=CURTAIN_MAX_EVENTS_VISIBLE,
        )
    if event_frames.size == 0:
        return

    row_count, column_count = surface_data.z_grid.shape
    frame_count = onset_history.size
    event_times = compute_event_times(frame_count, float(surface_data.y_grid[-1, 0]))
    frame_step_seconds = HOP_SIZE / float(SAMPLE_RATE)
    curtain_half_width = max(0.028, 0.30 * frame_step_seconds)
    z_top_limit = spec.z_max + spec.top_lift

    for frame_i, gate_peak, _diffuse_peak in zip(event_frames, event_gate_peaks, event_diffuse_peaks, strict=False):
        y_center = float(event_times[frame_i])
        surface_row = map_event_frame_to_surface_row(frame_i, frame_count, row_count)
        x_plane = np.vstack([surface_data.x_grid[surface_row], surface_data.x_grid[surface_row]])
        y_plane = np.vstack(
            [
                np.full(column_count, y_center - curtain_half_width, dtype=np.float32),
                np.full(column_count, y_center + curtain_half_width, dtype=np.float32),
            ]
        )
        top_line = np.minimum(
            surface_data.z_grid[surface_row] + spec.top_lift * (0.30 + 0.70 * gate_peak),
            z_top_limit,
        )
        z_plane = np.vstack(
            [
                np.full(column_count, spec.z_min, dtype=np.float32),
                top_line,
            ]
        )
        curtain_alpha = np.clip(0.025 + 0.135 * gate_peak, 0.0, 1.0)
        ridge_alpha = np.clip(0.10 + 0.62 * gate_peak, 0.0, 1.0)
        floor_alpha = np.clip(0.08 + 0.74 * gate_peak, 0.0, 1.0)

        axes.plot_surface(
            x_plane,
            y_plane,
            z_plane,
            rstride=1,
            cstride=1,
            color=to_rgba(WAVEFORM_ONSET_CURTAIN_FILL_COLOR, curtain_alpha),
            linewidth=0.0,
            antialiased=ANTIALIASED,
            shade=False,
        )
        plot3(
            axes,
            surface_data.x_grid[surface_row],
            np.full(column_count, y_center, dtype=np.float32),
            top_line,
            to_rgba(WAVEFORM_ONSET_CURTAIN_RIDGE_COLOR, ridge_alpha),
            WIREFRAME_SECONDARY_LINEWIDTH + 0.22 * gate_peak,
        )
        plot3(
            axes,
            surface_data.x_grid[surface_row],
            np.full(column_count, y_center, dtype=np.float32),
            np.full(column_count, spec.z_line, dtype=np.float32),
            to_rgba(WAVEFORM_ONSET_CURTAIN_FLOOR_COLOR, floor_alpha),
            WIREFRAME_PRIMARY_LINEWIDTH + 0.28 * gate_peak,
        )


def compute_brightness_wash_rows(onset_history: np.ndarray, y_values: np.ndarray, duration_seconds: float) -> np.ndarray:
    view_duration_seconds = min(duration_seconds, 60.0)
    frame_times = np.arange(onset_history.size, dtype=np.float32) * (HOP_SIZE / float(SAMPLE_RATE))
    valid = frame_times <= view_duration_seconds
    if not np.any(valid):
        valid = np.array([True] + [False] * (frame_times.size - 1), dtype=bool)
    visible_frame_times = frame_times[valid]
    visible_onset_history = onset_history[valid]
    if view_duration_seconds > 0.0:
        wash_row_times = np.linspace(0.0, view_duration_seconds, ONSET_WASH_DISPLAY_ROWS, dtype=np.float32)
        dense_wash_rows = resample_onset_gate_rows(wash_row_times, visible_frame_times, visible_onset_history)
        return np.interp(
            np.clip(y_values, 0.0, view_duration_seconds),
            wash_row_times,
            dense_wash_rows,
        ).astype(np.float32)
    return np.zeros_like(y_values, dtype=np.float32)


def build_brightness_wash_facecolors(surface: np.ndarray, wash_rows: np.ndarray) -> np.ndarray:
    rgba = np.zeros((*surface.shape, 4), dtype=np.float32)
    base_shading = compute_surface_shading(surface, 0.62, 0.22)
    visible_light = np.clip(wash_rows, 0.0, 1.0)
    visible_light_display = np.power(visible_light, 0.85)[:, None]
    shaded = np.clip(base_shading * (0.36 + 1.04 * visible_light_display), 0.0, 1.0)
    dark = WAVEFORM_BRIGHTNESS_WASH_DARK
    cyan = WAVEFORM_BRIGHTNESS_WASH_CYAN
    white = WAVEFORM_BRIGHTNESS_WASH_WHITE
    ramp = (1.0 - visible_light_display) * dark[None, :] + visible_light_display * (
        (1.0 - visible_light_display) * cyan[None, :] + visible_light_display * white[None, :]
    )
    rgba[..., :3] = np.clip(ramp[:, None, :] * shaded[..., None], 0.0, 1.0)
    rgba[..., 3] = np.clip(0.02 + 0.92 * np.power(visible_light, 1.15), 0.0, 1.0)[:, None]
    return rgba


def draw_chroma_wire_overlay(axes, surface_data: SurfaceData, onset_gate: np.ndarray, palette: ChromaPalette, lift: float) -> None:
    row_count, column_count = surface_data.z_grid.shape
    for row in range(0, row_count, max(1, row_count // WIREFRAME_ROW_SKIP)):
        strobe = float(onset_gate[row])
        plot3(
            axes,
            surface_data.x_grid[row],
            surface_data.y_grid[row],
            surface_data.z_grid[row] + lift,
            to_rgba(palette.row_wire_color, palette.row_wire_alpha + 0.18 * strobe),
            WIREFRAME_PRIMARY_LINEWIDTH + 0.18 * strobe,
        )
    for column in range(0, column_count, max(1, column_count // WIREFRAME_COLUMN_SKIP)):
        plot3(
            axes,
            surface_data.x_grid[:, column],
            surface_data.y_grid[:, column],
            surface_data.z_grid[:, column] + lift * 0.7,
            to_rgba(palette.column_wire_color, palette.column_wire_alpha),
            WIREFRAME_SECONDARY_LINEWIDTH,
        )


def draw_single_band_panel(axes, band_data: np.ndarray, profile: ColorProfile, duration_seconds: float, height_scale: float) -> None:
    axes.view_init(elev=VIEW_ELEVATION, azim=VIEW_AZIMUTH)
    axes.set_box_aspect(BOX_ASPECT, zoom=1.25)
    apply_axis_styling(
        axes=axes,
        tick_label_size=SMALL_TICK_LABEL_SIZE,
        tick_padding=SMALL_TICK_PADDING,
        x_tick_count=SMALL_X_TICK_COUNT,
        y_tick_count=SMALL_Y_TICK_COUNT,
        z_tick_count=SMALL_Z_TICK_COUNT,
        transparent_background=True,
    )
    normalized_band = normalize_band_surface(band_data)
    row_count, column_count = normalized_band.shape
    x_values = np.arange(column_count)
    y_values = np.linspace(0, duration_seconds, row_count)
    x_grid, y_grid = np.meshgrid(x_values, y_values)
    z_grid = normalized_band * height_scale
    render_surface(
        axes,
        SurfaceData(
            surface=normalized_band,
            position_surface=normalized_band,
            x_grid=x_grid,
            y_grid=y_grid,
            z_grid=z_grid,
            x_limits=(0.0, float(column_count - 1)),
            y_limits=(0.0, 60.0),
            z_limits=(0.0, 0.9),
            labels=(BAND_X_LABEL, BAND_Y_LABEL, BAND_Z_LABEL),
            height_scale=height_scale,
        ),
        apply_color_profile(normalized_band, profile),
        linewidth=SMALL_SURFACE_EDGE_LINEWIDTH,
        edgecolor=to_rgba(profile.edge_color, profile.edge_alpha),
    )
    draw_wireframe_overlay(
        axes=axes,
        x_grid=x_grid,
        y_grid=y_grid,
        z_grid=z_grid,
        profile=profile,
        height_scale=height_scale,
        row_count=row_count,
        column_count=column_count,
        row_skip=SMALL_WIREFRAME_ROW_SKIP,
        column_skip=SMALL_WIREFRAME_COLUMN_SKIP,
        primary_linewidth=SMALL_WIREFRAME_PRIMARY_LINEWIDTH,
        secondary_linewidth=SMALL_WIREFRAME_SECONDARY_LINEWIDTH,
    )
    axes.set_xlim(0, column_count - 1)
    axes.set_ylim(0, 60)
    axes.set_zlim(0, 0.9)
    set_axis_labels(
        axes,
        BAND_X_LABEL,
        BAND_Y_LABEL,
        BAND_Z_LABEL,
        SMALL_AXIS_LABEL_SIZE,
        x_labelpad=SMALL_AXIS_LABEL_PADDING_X,
        y_labelpad=SMALL_AXIS_LABEL_PADDING_Y,
        z_labelpad=SMALL_AXIS_LABEL_PADDING_Z,
    )


def draw_merged_bands_panel(
    axes,
    low_band: np.ndarray,
    mid_band: np.ndarray,
    high_band: np.ndarray,
    profile: ColorProfile,
    duration_seconds: float,
) -> None:
    axes.view_init(elev=VIEW_ELEVATION, azim=VIEW_AZIMUTH)
    axes.set_box_aspect(BOX_ASPECT, zoom=1.20)
    apply_axis_styling(
        axes=axes,
        tick_label_size=TICK_LABEL_SIZE,
        tick_padding=TICK_PADDING,
        x_tick_count=X_TICK_COUNT,
        y_tick_count=Y_TICK_COUNT,
        z_tick_count=Z_TICK_COUNT,
    )
    x_offset = 0
    bands_config = [
        (low_band, LOW_BAND_POINT_COUNT, MERGED_LOW_SCALE),
        (mid_band, MID_BAND_POINT_COUNT, MERGED_MID_SCALE),
        (high_band, HIGH_BAND_POINT_COUNT, MERGED_HIGH_SCALE),
    ]
    for raw_band_data, point_count, height_scale in bands_config:
        normalized_band = normalize_band_surface(raw_band_data)
        row_count = normalized_band.shape[0]
        x_values = np.linspace(x_offset, x_offset + normalized_band.shape[1] - 1, normalized_band.shape[1])
        y_values = np.linspace(0, duration_seconds, row_count)
        x_grid, y_grid = np.meshgrid(x_values, y_values)
        z_grid = normalized_band * height_scale
        render_surface(
            axes,
            SurfaceData(
                surface=normalized_band,
                position_surface=normalized_band,
                x_grid=x_grid,
                y_grid=y_grid,
                z_grid=z_grid,
                x_limits=(0.0, 60.0),
                y_limits=(0.0, 60.0),
                z_limits=(0.0, 1.2),
                labels=(MERGED_X_LABEL, MERGED_Y_LABEL, MERGED_Z_LABEL),
                height_scale=height_scale,
            ),
            apply_color_profile(normalized_band, profile),
            linewidth=SURFACE_EDGE_LINEWIDTH,
            edgecolor=to_rgba(profile.edge_color, max(profile.edge_alpha, 0.06)),
        )
        x_offset += point_count + BAND_GAP
    axes.set_xlim(0, 60)
    axes.set_ylim(0, 60)
    axes.set_zlim(0, 1.2)
    set_axis_labels(axes, MERGED_X_LABEL, MERGED_Y_LABEL, MERGED_Z_LABEL, AXIS_LABEL_SIZE)


def compute_waveform_rms_surface(samples: np.ndarray) -> tuple[np.ndarray, np.ndarray, int]:
    samples_per_lane_point = FFT_WINDOW_SIZE // RUNTIME_POINTS
    starts = audio_frame_starts(samples)
    abs_mean = np.zeros((len(starts), RUNTIME_POINTS), dtype=np.float32)
    rms = np.zeros_like(abs_mean)
    for frame_i, frame in enumerate(iter_audio_frames(samples)):
        for point_i in range(RUNTIME_POINTS):
            a = point_i * samples_per_lane_point
            b = min(a + samples_per_lane_point, FFT_WINDOW_SIZE)
            cell = frame[a:b]
            abs_mean[frame_i, point_i] = float(np.mean(np.abs(cell)))
            rms[frame_i, point_i] = float(np.sqrt(np.mean(cell * cell)))

    def aggregate_time(field: np.ndarray, lanes: int) -> np.ndarray:
        edges = compute_bin_edges(field.shape[0], lanes)
        out = np.zeros((lanes, field.shape[1]), dtype=np.float32)
        for i, (a, b) in enumerate(zip(edges[:-1], edges[1:], strict=False)):
            block = field[a : max(a + 1, b), :]
            out[i] = 0.72 * block.mean(axis=0) + 0.28 * np.percentile(block, 88.0, axis=0)
        return out

    amp_33 = aggregate_time(abs_mean, TOTAL_LANES)
    rms_33 = aggregate_time(rms, TOTAL_LANES)
    amp = interp_rows(amp_33, WAVEFORM_POINTS, grid_dtype=None)
    rms_f = interp_rows(rms_33, WAVEFORM_POINTS, grid_dtype=None)
    try:
        from scipy.ndimage import gaussian_filter

        amp = gaussian_filter(amp, sigma=(0.70, 0.55), mode="nearest").astype(np.float32)
        rms_f = gaussian_filter(rms_f, sigma=(0.70, 0.55), mode="nearest").astype(np.float32)
    except Exception:
        pass
    amp_scale = max(float(np.percentile(amp, 99.7)), 1.0e-9)
    height = np.clip(amp / amp_scale, 0.0, 1.0)
    height = np.power(height, 0.72).astype(np.float32)
    rms_low = float(np.percentile(rms_f, 54.0))
    rms_high = max(float(np.percentile(rms_f, 99.5)), rms_low + 1.0e-9)
    rms_norm = np.clip((rms_f - rms_low) / (rms_high - rms_low), 0.0, 1.0)
    rms_norm = np.power(rms_norm, 0.62).astype(np.float32)
    return height, rms_norm, len(starts)


def build_waveform_rms_facecolors(rms_norm: np.ndarray, z_for_shade: np.ndarray) -> np.ndarray:
    rgba = WAVEFORM_RMS_COLORMAP(np.clip(rms_norm, 0.0, 1.0))
    shade = compute_surface_shading(z_for_shade, 0.72, 0.26)
    rgba[..., :3] = np.clip(rgba[..., :3] * shade[..., None], 0.0, 1.0)
    rgba[..., 3] = 1.0
    return rgba


def draw_rms_wireframe_overlay(axes, surface_data: SurfaceData) -> None:
    row_count, column_count = surface_data.z_grid.shape
    lift = 0.018 * WAVEFORM_HEIGHT_SCALE
    for i in range(0, row_count, max(1, row_count // WIREFRAME_ROW_SKIP)):
        plot3(
            axes,
            surface_data.x_grid[i],
            surface_data.y_grid[i],
            surface_data.z_grid[i] + lift,
            to_rgba(WAVEFORM_RMS_ROW_WIRE_COLOR, 0.15),
            WIREFRAME_PRIMARY_LINEWIDTH,
        )
    for j in range(0, column_count, max(1, column_count // WIREFRAME_COLUMN_SKIP)):
        plot3(
            axes,
            surface_data.x_grid[:, j],
            surface_data.y_grid[:, j],
            surface_data.z_grid[:, j] + lift * 0.7,
            to_rgba(WAVEFORM_RMS_COLUMN_WIRE_COLOR, 0.08),
            WIREFRAME_SECONDARY_LINEWIDTH,
        )


def build_triangle_source_vertex_indices(row_count: int, column_count: int) -> np.ndarray:
    indices = np.arange(row_count * column_count, dtype=np.int32).reshape(row_count, column_count)
    idx00 = indices[:-1, :-1].reshape(-1)
    idx10 = indices[1:, :-1].reshape(-1)
    idx01 = indices[:-1, 1:].reshape(-1)
    idx11 = indices[1:, 1:].reshape(-1)
    source = np.empty((idx00.size * 2, 3), dtype=np.int32)
    source[0::2] = np.stack((idx10, idx01, idx00), axis=1)
    source[1::2] = np.stack((idx10, idx11, idx01), axis=1)
    return source


def project_binary_survival_history(
    survival_history: np.ndarray,
    *,
    target_lanes: int = TOTAL_LANES,
    target_points: int = RUNTIME_POINTS,
) -> np.ndarray:
    occupancy = aggregate_along_axis(survival_history, target_lanes, 0, mean_weight=1.0)
    return interp_rows(occupancy, target_points)


def build_hilbert_overlay_collections(
    surface_data: SurfaceData,
    hilbert_survival_surface: np.ndarray,
    *,
    render_epsilon: float,
) -> tuple[Poly3DCollection, Poly3DCollection]:
    triangles, _ = build_triangle_soup(surface_data.x_grid, surface_data.y_grid, surface_data.z_grid, None)
    row_count, column_count = surface_data.z_grid.shape
    source_vertex_indices = build_triangle_source_vertex_indices(row_count, column_count)
    wire_rgba = np.array(
        to_rgba(WAVEFORM_HILBERT_BASE_WIRE_COLOR, WAVEFORM_HILBERT_BASE_WIRE_ALPHA),
        dtype=np.float32,
    )
    base_wire = Poly3DCollection(
        triangles,
        facecolors=np.zeros((triangles.shape[0], 4), dtype=np.float32),
        edgecolors=np.tile(wire_rgba, (triangles.shape[0], 1)),
        linewidths=0.22,
        antialiaseds=ANTIALIASED,
        zsort="average",
        shade=False,
    )
    survival_occupancy = hilbert_survival_surface.reshape(-1)
    face_alpha = survival_occupancy[source_vertex_indices].mean(axis=1).astype(np.float32)
    face_visible = face_alpha > 0.0
    if not np.any(face_visible):
        empty_faces = Poly3DCollection(
            np.asarray([triangles[0]], dtype=np.float32),
            facecolors=np.zeros((1, 4), dtype=np.float32),
            edgecolors=np.zeros((1, 4), dtype=np.float32),
            linewidths=0.0,
            antialiaseds=ANTIALIASED,
            zsort="average",
            shade=False,
        )
        return base_wire, empty_faces
    render_triangles = triangles.copy()
    render_triangles[:, :, 2] += render_epsilon
    vertex_colors = build_runtime_vertex_rgba(row_count, column_count).reshape(-1, 4)
    face_colors = vertex_colors[source_vertex_indices].mean(axis=1)
    face_shading = compute_triangle_flat_shading(
        render_triangles,
        floor=DREAMCAST_CYAN.shading_floor,
        gain=DREAMCAST_CYAN.shading_gain,
    )
    face_colors[:, :3] = np.clip(face_colors[:, :3] * face_shading[:, None], 0.0, 1.0)
    face_colors[:, 3] = face_alpha
    overlay_triangles = render_triangles[face_visible]
    overlay_facecolors = face_colors[face_visible]
    overlay_edgecolors = overlay_facecolors.copy()
    overlay_edgecolors[:, :3] = np.clip(overlay_edgecolors[:, :3] * 1.08 + 0.03, 0.0, 1.0)
    overlay_edgecolors[:, 3] = face_alpha[face_visible]
    overlay_faces = Poly3DCollection(
        overlay_triangles,
        facecolors=overlay_facecolors,
        edgecolors=overlay_edgecolors,
        linewidths=0.30,
        antialiaseds=ANTIALIASED,
        zsort="average",
        shade=False,
    )
    return base_wire, overlay_faces


def smoothstep01(values: np.ndarray | float) -> np.ndarray | float:
    values = np.clip(values, 0.0, 1.0)
    return values * values * (3.0 - 2.0 * values)


def compute_spectral_flatness_alpha(raw_magnitudes: np.ndarray) -> np.ndarray:
    bin_min = int(HIGH_BAND_FLATNESS_BOUNDS[0, 0])
    bin_max = int(HIGH_BAND_FLATNESS_BOUNDS[-1, 1])
    powers = np.maximum(raw_magnitudes[:, bin_min : bin_max + 1] ** 2, SPECTRAL_FLATNESS_AMIN).astype(np.float32)
    arithmetic_mean = powers.mean(axis=1).astype(np.float32)
    geometric_mean = np.exp(np.log(powers).mean(axis=1)).astype(np.float32)
    flatness = np.clip(geometric_mean / np.maximum(arithmetic_mean, SPECTRAL_FLATNESS_AMIN), 0.0, 1.0).astype(np.float32)
    presence = np.ones_like(flatness, dtype=np.float32)
    if SPECTRAL_FLATNESS_GLITTER_SILENCE_GATE_POWER_HIGH > 0.0:
        gate_low = SPECTRAL_FLATNESS_GLITTER_SILENCE_GATE_POWER_LOW
        gate_high = SPECTRAL_FLATNESS_GLITTER_SILENCE_GATE_POWER_HIGH
        if gate_high > gate_low:
            presence = smoothstep01((arithmetic_mean - gate_low) / (gate_high - gate_low)).astype(np.float32)
        else:
            presence = np.where(arithmetic_mean > gate_low, 1.0, 0.0).astype(np.float32)
    target = np.clip(flatness * presence, 0.0, 1.0).astype(np.float32)
    alpha = np.zeros_like(flatness, dtype=np.float32)
    smooth = 0.0
    for frame_i, target_i in enumerate(target):
        target_i = float(target_i)
        rate = SPECTRAL_FLATNESS_GLITTER_ATTACK_RATE if target_i > smooth else SPECTRAL_FLATNESS_GLITTER_RELEASE_RATE
        smooth = (1.0 - rate) * smooth + rate * target_i
        alpha[frame_i] = np.clip(smooth, 0.0, 1.0)
    return alpha


def compute_high_band_surface(raw_magnitudes: np.ndarray) -> np.ndarray:
    high_history = np.zeros((raw_magnitudes.shape[0], HIGH_BAND_FLATNESS_BOUNDS.shape[0]), dtype=np.float32)
    adaptive_peak = 1.0e-12
    for frame_i, magnitudes in enumerate(raw_magnitudes):
        lane_values = np.array(
            [magnitudes[start : end + 1].mean() for start, end in HIGH_BAND_FLATNESS_BOUNDS],
            dtype=np.float32,
        )
        band_peak = float(lane_values.max(initial=0.0))
        adaptive_peak = max(adaptive_peak * HIGH_BAND_ADAPTIVE_PEAK_DECAY, band_peak, 1.0e-12)
        db_below_peak = np.log(np.maximum(lane_values / adaptive_peak, 1.0e-12)) * DECIBEL_SCALE_FACTOR
        high_history[frame_i] = np.clip(1.0 + db_below_peak / HIGH_BAND_ADAPTIVE_DYNAMIC_RANGE_DB, 0.0, 1.0)
    return aggregate_along_axis(high_history, TOTAL_LANES, axis=0, mean_weight=1.0)


def project_alpha_rows(alpha_history: np.ndarray, row_count: int) -> np.ndarray:
    row_signal = np.asarray(alpha_history, dtype=np.float32).reshape(-1, 1)
    return aggregate_along_axis(row_signal, row_count, axis=0, mean_weight=1.0)[:, 0].astype(np.float32)


def build_high_band_surface_data(surface: np.ndarray, duration_seconds: float) -> SurfaceData:
    return make_surface_data(
        surface,
        compute_high_band_center_hz(HIGH_BAND_FLATNESS_BOUNDS),
        duration_seconds,
        HIGH_BAND_HEIGHT_SCALE,
        (0.0, HIGH_BAND_GLITTER_Z_LIMIT),
        (FFT_X_LABEL, FFT_Y_LABEL, FFT_Z_LABEL),
    )


def draw_constant_density_glitter(axes, surface_data: SurfaceData, row_alpha: np.ndarray) -> int:
    surface = surface_data.surface
    row_count, column_count = surface.shape
    rng = np.random.default_rng(HIGH_BAND_GLITTER_RANDOM_SEED)
    x_values = compute_high_band_center_hz(HIGH_BAND_FLATNESS_BOUNDS)
    y_values = surface_data.y_grid[:, 0].astype(np.float32)
    y_step = float(y_values[1] - y_values[0]) if row_count > 1 else 1.0
    x_step = float(np.min(np.diff(x_values))) if column_count > 1 else 1.0
    xs: list[float] = []
    ys: list[float] = []
    zs: list[float] = []
    alphas: list[float] = []
    for row_i in range(row_count):
        row_strength = float(np.clip(row_alpha[row_i], 0.0, 1.0))
        for col_i in range(column_count):
            base_z = float(surface[row_i, col_i] * HIGH_BAND_HEIGHT_SCALE)
            for _ in range(HIGH_BAND_GLITTER_SUBPOINTS_PER_CELL):
                if float(rng.random()) > HIGH_BAND_GLITTER_NOISE_DENSITY:
                    continue
                xs.append(float(x_values[col_i] + rng.uniform(-HIGH_BAND_GLITTER_X_JITTER, HIGH_BAND_GLITTER_X_JITTER) * x_step))
                ys.append(float(y_values[row_i] + rng.uniform(-HIGH_BAND_GLITTER_Y_JITTER, HIGH_BAND_GLITTER_Y_JITTER) * y_step))
                zs.append(float(base_z + HIGH_BAND_GLITTER_Z_BASE_OFFSET + rng.uniform(0.0, HIGH_BAND_GLITTER_Z_SPREAD)))
                texture_alpha = 0.55 + 0.45 * float(rng.random())
                alphas.append(float(np.clip(row_strength * texture_alpha, 0.0, 1.0)))
    colors = np.ones((len(xs), 4), dtype=np.float32)
    colors[:, 3] = np.asarray(alphas, dtype=np.float32)
    axes.scatter(
        np.asarray(xs, dtype=np.float32),
        np.asarray(ys, dtype=np.float32),
        np.asarray(zs, dtype=np.float32),
        s=HIGH_BAND_GLITTER_POINT_SIZE,
        marker="s",
        c=colors,
        depthshade=False,
        linewidths=0,
    )
    return len(xs)


def _finish_render(surface_data: SurfaceData, output_image_file: Path, draw, spec: RenderSpec = RenderSpec()):
    figure, axes = create_3d_figure(spec)
    draw(axes)
    apply_surface_axes(axes, surface_data)
    finish_figure(figure, output_image_file)


def _waveform_plot(
    filename: str,
    *,
    runtime: RuntimeSettings | None = None,
    audio: AudioData | None = None,
    extra_top: float = 0.0,
    surface: np.ndarray | None = None,
):
    runtime = DEFAULT_WAVEFORM_RUNTIME if runtime is None else runtime
    audio = load_default_audio() if audio is None else audio
    if surface is None:
        surface = compute_waveform_surface(audio.samples)
    surface_data = build_waveform_surface_data(
        audio.samples,
        audio.duration_seconds,
        signed=runtime.waveform_signed,
        unsigned_surface=surface,
        extra_top=extra_top,
    )
    return (
        runtime,
        audio,
        surface,
        surface_data,
        make_output_path(filename, signed=runtime.waveform_signed),
    )


def _brightness_wash_rows(onset_history: np.ndarray, surface_data: SurfaceData, audio: AudioData):
    return compute_brightness_wash_rows(
        onset_history,
        surface_data.y_grid[:, 0].astype("float32"),
        audio.duration_seconds,
    )


def run_waveform_terrain(runtime: RuntimeSettings | None = None) -> None:
    runtime, _audio, _surface, surface_data, output = _waveform_plot("waveform_terrain.png", runtime=runtime)
    _finish_render(
        surface_data,
        output,
        lambda axes: render_profile_surface_with_wireframe(axes, surface_data, runtime.surface_profile),
    )


def run_waveform_onset_scribbles(runtime: RuntimeSettings | None = None) -> None:
    scribble_lift = 0.034 * WAVEFORM_HEIGHT_SCALE
    _runtime, audio, _surface, surface_data, output = _waveform_plot(
        "waveform_terrain_onset_scribbles.png",
        runtime=runtime,
        extra_top=scribble_lift * 1.2,
    )
    onset_rows = project_scalar_history(compute_waveform_onset_history(audio.samples))
    spec = ScribbleSpec(lift=scribble_lift)
    _finish_render(
        surface_data,
        output,
        lambda axes: draw_onset_scribbles(axes, surface_data, onset_rows, spec),
    )


def run_waveform_onset_brightness_wash(runtime: RuntimeSettings | None = None) -> None:
    _runtime, audio, waveform_surface, surface_data, output = _waveform_plot("waveform_terrain_onset_brightness_wash.png", runtime=runtime)
    onset_history = compute_waveform_onset_history(audio.samples)
    wash_rows = _brightness_wash_rows(onset_history, surface_data, audio)
    _finish_render(
        surface_data,
        output,
        lambda axes: render_surface(
            axes,
            surface_data,
            build_brightness_wash_facecolors(waveform_surface, wash_rows),
            linewidth=0.0,
        ),
    )


def run_waveform_onset_curtain(runtime: RuntimeSettings | None = None) -> None:
    curtain_top_lift = 0.18 * WAVEFORM_HEIGHT_SCALE
    _runtime, audio, _surface, surface_data, output = _waveform_plot(
        "waveform_terrain_onset_curtain.png",
        runtime=runtime,
        extra_top=curtain_top_lift,
    )
    onset_history = compute_waveform_onset_history(audio.samples)
    spec = CurtainSpec(
        top_lift=curtain_top_lift,
        z_min=WAVEFORM_Z_MIN,
        z_max=WAVEFORM_Z_MAX,
        z_line=WAVEFORM_Z_MIN + 0.0025,
    )
    _finish_render(
        surface_data,
        output,
        lambda axes: draw_onset_curtains(
            axes,
            surface_data,
            onset_history,
            spec,
        ),
    )


def run_waveform_rms(runtime: RuntimeSettings | None = None) -> None:
    audio = load_default_audio()
    height, rms_norm, _frame_count = compute_waveform_rms_surface(audio.samples)
    _runtime, _audio, _surface, surface_data, output = _waveform_plot(
        "waveform_terrain_rms.png",
        runtime=runtime,
        audio=audio,
        surface=height,
    )
    _finish_render(
        surface_data,
        output,
        lambda axes: (
            render_surface(
                axes,
                surface_data,
                build_waveform_rms_facecolors(rms_norm, height),
                linewidth=SURFACE_EDGE_LINEWIDTH,
                edgecolor=to_rgba(WAVEFORM_RMS_EDGE_COLOR, WAVEFORM_RMS_EDGE_ALPHA),
            ),
            draw_rms_wireframe_overlay(axes, surface_data),
        ),
    )


def run_waveform_hilbert_overlay(runtime: RuntimeSettings | None = None) -> None:
    runtime = DEFAULT_WAVEFORM_RUNTIME if runtime is None else runtime
    output_image_file = make_output_path(
        "waveform_terrain_hilbert_overlay.png",
        signed=runtime.waveform_signed,
    )
    audio = load_default_audio()
    render_epsilon = 0.001 * WAVEFORM_HEIGHT_SCALE
    position_surface = compute_waveform_position_surface(
        audio.samples,
        target_points=RUNTIME_POINTS,
        signed=runtime.waveform_signed,
    )
    surface_data = make_surface_data(
        position_surface,
        np.arange(position_surface.shape[1], dtype=np.float32),
        audio.duration_seconds,
        WAVEFORM_HEIGHT_SCALE,
        get_waveform_z_limits(extra_top=render_epsilon, signed=runtime.waveform_signed),
        (WAVEFORM_X_LABEL, WAVEFORM_Y_LABEL, WAVEFORM_Z_LABEL),
        position_surface=position_surface,
    )
    hilbert_history = compute_waveform_hilbert_mask_history(audio.samples)
    vertex_survival_history = (hilbert_history > 0.20).astype(np.float32)
    hilbert_survival_surface = project_binary_survival_history(
        vertex_survival_history,
        target_points=RUNTIME_POINTS,
    )
    figure, axes = create_3d_figure()
    base_wire, overlay_faces = build_hilbert_overlay_collections(
        surface_data,
        hilbert_survival_surface,
        render_epsilon=render_epsilon,
    )
    axes.add_collection3d(base_wire)
    axes.add_collection3d(overlay_faces)
    apply_surface_axes(axes, surface_data)
    finish_figure(figure, output_image_file)


def run_fft_terrain() -> None:
    _audio, _raw_magnitudes, _smoothed_levels, _surface, surface_data = _fft_plot()
    _finish_render(
        surface_data,
        make_output_path("fft_terrain.png"),
        lambda axes: render_profile_surface_with_wireframe(axes, surface_data, SELECTED_COLOR_PROFILE),
    )


def _fft_plot(*, low_band: bool = False, extra_top: float = 0.0):
    audio = load_default_audio()
    raw_magnitudes, smoothed_levels = compute_fft_history(audio.samples)
    if low_band:
        surface, _mid_band, _high_band = compute_frequency_bands(raw_magnitudes, smoothed_levels)
        surface_data = build_low_band_surface_data(surface, audio.duration_seconds, extra_top=extra_top)
    else:
        surface = compute_fft_surface(smoothed_levels)
        surface_data = build_fft_surface_data(surface, audio.duration_seconds, extra_top=extra_top)
    return audio, raw_magnitudes, smoothed_levels, surface, surface_data


def _fft_onset(raw_magnitudes: np.ndarray, smoothed_levels: np.ndarray, *, low_band: bool):
    if low_band:
        return compute_low_band_onset_history(raw_magnitudes, smoothed_levels)
    return compute_fft_onset_history(raw_magnitudes, smoothed_levels, use_raw=False)


def run_fft_onset_scribbles(*, low_band: bool = False) -> None:
    lift = (0.046 * LOW_BAND_HEIGHT_SCALE) if low_band else (0.020 * FFT_HEIGHT_SCALE)
    _audio, raw_magnitudes, smoothed_levels, _surface, surface_data = _fft_plot(
        low_band=low_band,
        extra_top=lift * 1.2,
    )
    onset_rows = project_scalar_history(_fft_onset(raw_magnitudes, smoothed_levels, low_band=low_band))
    output = "fft_low_band_terrain_onset_scribbles.png" if low_band else "fft_terrain_onset_scribbles.png"
    spec = ScribbleSpec(lift=lift)
    _finish_render(
        surface_data,
        make_output_path(output),
        lambda axes: draw_onset_scribbles(axes, surface_data, onset_rows, spec),
    )


def run_fft_onset_brightness_wash(*, low_band: bool = False) -> None:
    audio, raw_magnitudes, smoothed_levels, surface, surface_data = _fft_plot(low_band=low_band)
    onset_history = _fft_onset(raw_magnitudes, smoothed_levels, low_band=low_band)
    face_surface = surface_data.surface if low_band else surface
    wash_rows = _brightness_wash_rows(onset_history, surface_data, audio)
    output = "fft_low_band_terrain_onset_brightness_wash.png" if low_band else "fft_terrain_onset_brightness_wash.png"
    _finish_render(
        surface_data,
        make_output_path(output),
        lambda axes: render_surface(
            axes,
            surface_data,
            build_brightness_wash_facecolors(face_surface, wash_rows),
            linewidth=0.0,
        ),
    )


def run_fft_onset_curtain(*, low_band: bool = False) -> None:
    if low_band:
        curtain_lift = 0.21 * LOW_BAND_HEIGHT_SCALE
        spec = CurtainSpec(top_lift=curtain_lift, z_min=0.0, z_max=LOW_BAND_Z_MAX, z_line=0.004)
        output = "fft_low_band_terrain_onset_curtain.png"
    else:
        curtain_lift = 0.105 * FFT_HEIGHT_SCALE
        spec = CurtainSpec(
            top_lift=curtain_lift,
            z_min=FFT_Z_MIN,
            z_max=FFT_Z_MAX,
            z_line=FFT_Z_MIN + 0.004,
        )
        output = "fft_terrain_onset_curtain.png"
    _audio, raw_magnitudes, smoothed_levels, _surface, surface_data = _fft_plot(
        low_band=low_band,
        extra_top=spec.top_lift,
    )
    onset_history = _fft_onset(raw_magnitudes, smoothed_levels, low_band=low_band)
    _finish_render(
        surface_data,
        make_output_path(output),
        lambda axes: draw_onset_curtains(axes, surface_data, onset_history, spec),
    )


def run_fft_bands_terrain() -> None:
    audio = load_default_audio()
    raw_magnitudes, smoothed_levels = compute_fft_history(audio.samples)
    low_band, mid_band, high_band = compute_frequency_bands(raw_magnitudes, smoothed_levels)
    figure, gridspec = create_bands_figure()
    draw_merged_bands_panel(
        figure.add_subplot(gridspec[:, 0], projection="3d"),
        low_band,
        mid_band,
        high_band,
        SELECTED_COLOR_PROFILE,
        audio.duration_seconds,
    )
    for index, (band, height_scale) in enumerate(
        (
            (low_band, LOW_BAND_HEIGHT_SCALE),
            (mid_band, MID_BAND_HEIGHT_SCALE),
            (high_band, HIGH_BAND_HEIGHT_SCALE),
        )
    ):
        draw_single_band_panel(
            figure.add_subplot(gridspec[index, 1], projection="3d"),
            band,
            SELECTED_COLOR_PROFILE,
            audio.duration_seconds,
            height_scale,
        )
    finish_bands_figure(figure, make_output_path("fft_bands_terrain.png"))


def run_fft_terrain_chroma() -> None:
    audio = load_default_audio()
    raw_magnitudes, smoothed_levels = compute_fft_history(audio.samples)
    ridge_history = compute_fft_ridge_history(smoothed_levels)
    surface = compute_fft_surface(smoothed_levels)
    chroma_frame_history, chroma_mask_history = compute_chroma_history(raw_magnitudes, ridge_history)
    chroma_ids, chroma_mask = aggregate_chroma_history(
        chroma_frame_history,
        chroma_mask_history,
        TOTAL_LANES,
    )
    onset_gate = aggregate_along_axis(
        compute_fft_onset_history(raw_magnitudes, smoothed_levels, use_raw=True)[:, None],
        TOTAL_LANES,
        0,
    )[:, 0]
    surface_data = build_fft_surface_data(surface, audio.duration_seconds)
    figure, axes = create_3d_figure()
    palette = SELECTED_CHROMA_PALETTE
    render_surface(
        axes,
        surface_data,
        apply_chroma_palette(surface, chroma_ids, chroma_mask, palette),
        linewidth=SURFACE_EDGE_LINEWIDTH,
        edgecolor=to_rgba(palette.edge_color, palette.edge_alpha),
    )
    draw_chroma_wire_overlay(axes, surface_data, onset_gate, palette, 0.018 * FFT_HEIGHT_SCALE)
    apply_surface_axes(axes, surface_data)
    finish_figure(figure, make_output_path("fft_terrain_chroma.png"))


def run_fft_bands_terrain_chroma() -> None:
    audio = load_default_audio()
    raw_magnitudes, smoothed_levels = compute_fft_history(audio.samples)
    _low_band_history, mid_band_history, _high_band_history = compute_frequency_band_histories(
        raw_magnitudes,
        smoothed_levels,
    )
    mid_surface = normalize_surface(aggregate_along_axis(mid_band_history, TOTAL_LANES, 0), 99.5, 0.78)
    chroma_frame_history, chroma_mask_history = compute_chroma_history(
        raw_magnitudes,
        np.clip(mid_band_history / 3.0, 0.0, 1.0),
    )
    chroma_ids, chroma_mask = aggregate_chroma_history(
        chroma_frame_history,
        chroma_mask_history,
        TOTAL_LANES,
    )
    onset_gate = aggregate_along_axis(
        compute_fft_onset_history(raw_magnitudes, smoothed_levels, use_raw=True)[:, None],
        TOTAL_LANES,
        0,
    )[:, 0]
    surface_data = build_indexed_band_surface_data(
        mid_surface,
        audio.duration_seconds,
        MID_BAND_HEIGHT_SCALE,
        (0.0, 3.0),
        (BAND_X_LABEL, FFT_Y_LABEL, BAND_Z_LABEL),
    )
    figure, axes = create_3d_figure()
    palette = SELECTED_CHROMA_PALETTE
    render_surface(
        axes,
        surface_data,
        apply_chroma_palette(mid_surface, chroma_ids, chroma_mask, palette),
        linewidth=SURFACE_EDGE_LINEWIDTH,
        edgecolor=to_rgba(palette.edge_color, palette.edge_alpha),
    )
    draw_chroma_wire_overlay(axes, surface_data, onset_gate, palette, 0.018 * MID_BAND_HEIGHT_SCALE)
    apply_surface_axes(axes, surface_data)
    finish_figure(figure, make_output_path("fft_bands_terrain_chroma.png"))


def run_fft_high_band_spectral_flatness() -> None:
    audio = load_default_audio()
    raw_magnitudes, _smoothed_levels = compute_fft_history(audio.samples)
    surface = compute_high_band_surface(raw_magnitudes)
    row_alpha = project_alpha_rows(compute_spectral_flatness_alpha(raw_magnitudes), surface.shape[0])
    surface_data = build_high_band_surface_data(surface, audio.duration_seconds)
    figure, axes = create_3d_figure(RenderSpec(transparent_background=True))
    draw_constant_density_glitter(axes, surface_data, row_alpha)
    apply_surface_axes(axes, surface_data)
    finish_figure(figure, make_output_path("fft_high_band_terrain_spectral_flatness.png"))


def main() -> None:
    run_fft_bands_terrain()
    run_fft_bands_terrain_chroma()
    run_fft_high_band_spectral_flatness()
    run_fft_onset_brightness_wash(low_band=True)
    run_fft_onset_curtain(low_band=True)
    run_fft_onset_scribbles(low_band=True)
    run_fft_terrain()
    run_fft_terrain_chroma()
    run_fft_onset_brightness_wash()
    run_fft_onset_curtain()
    run_fft_onset_scribbles()
    run_waveform_terrain(DEFAULT_WAVEFORM_RUNTIME)
    run_waveform_hilbert_overlay(DEFAULT_WAVEFORM_RUNTIME)
    run_waveform_onset_brightness_wash(DEFAULT_WAVEFORM_RUNTIME)
    run_waveform_onset_curtain(DEFAULT_WAVEFORM_RUNTIME)
    run_waveform_onset_scribbles(DEFAULT_WAVEFORM_RUNTIME)
    run_waveform_rms(DEFAULT_WAVEFORM_RUNTIME)
    run_waveform_terrain(SIGNED_WAVEFORM_RUNTIME)
    run_waveform_hilbert_overlay(SIGNED_WAVEFORM_RUNTIME)
    run_waveform_onset_brightness_wash(SIGNED_WAVEFORM_RUNTIME)
    run_waveform_onset_curtain(SIGNED_WAVEFORM_RUNTIME)
    run_waveform_onset_scribbles(SIGNED_WAVEFORM_RUNTIME)
    run_waveform_rms(SIGNED_WAVEFORM_RUNTIME)


if __name__ == "__main__":
    main()
