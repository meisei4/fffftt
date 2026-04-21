import numpy as np
import matplotlib.pyplot as plt
from scipy.signal import chirp
from pathlib import Path


OUTDIR = Path.home() / "fffftt"


def build_analytic_envelope_fir(analysis_window_samples):
    hilbert_filter_length = 100

    analysis_window_samples = np.asarray(analysis_window_samples, dtype=float)
    analysis_mean = analysis_window_samples.mean()
    centered_samples = analysis_window_samples - analysis_mean

    hilbert_filter_positions = 0.5 * np.arange(
        (1 - hilbert_filter_length) / 2,
        (hilbert_filter_length - 1) / 2 + 1,
    )
    hilbert_filter = np.sinc(hilbert_filter_positions) * np.exp(1j * np.pi * hilbert_filter_positions)
    hilbert_filter *= np.kaiser(hilbert_filter_length, 8.0)
    hilbert_filter /= np.sum(np.real(hilbert_filter))

    local_analytic_envelope_value = np.abs(np.convolve(centered_samples, hilbert_filter, mode="same"))
    local_upper_envelope_value = analysis_mean + local_analytic_envelope_value
    local_lower_envelope_value = analysis_mean - local_analytic_envelope_value

    return local_upper_envelope_value, local_lower_envelope_value


def main():
    OUTDIR.mkdir(parents=True, exist_ok=True)

    t = np.arange(0, 3.001, 1 / 1000.0)
    q1 = np.sin(2 * np.pi * 7 * t) * np.exp(-t / 2)
    q2 = chirp(t, f0=30, t1=2, f1=5, method="linear") * np.exp(-(2 * t - 3) ** 2) + 2.0

    up1, lo1 = build_analytic_envelope_fir(q1)
    up2, lo2 = build_analytic_envelope_fir(q2)

    fig = plt.figure(figsize=(10, 5))
    plt.plot(t, q1, label="q1")
    plt.plot(t, q2, label="q2")
    plt.plot(t, up1, label="up1")
    plt.plot(t, lo1, label="lo1")
    plt.plot(t, up2, label="up2")
    plt.plot(t, lo2, label="lo2")
    plt.xlim(0, 3)
    plt.title("MATLAB-style analytic envelope via FIR Hilbert filter (fl=100)")
    plt.xlabel("t")
    plt.ylabel("amplitude")
    plt.legend()
    fig.tight_layout()
    fig.savefig(OUTDIR / "analytic_envelope_matlab.png", dpi=160)
    plt.close(fig)


if __name__ == "__main__":
    main()
