/*
 * SEAME — UFLDv2 post-processing C++ extension
 *
 * Usa OpenBLAS (cblas_sgemv) para FC1 e FC2.
 * O GIL é libertado durante o cálculo → main thread (YHailo) não bloqueia.
 *
 * Build: cd postprocess_cpp && python3 setup.py build_ext --inplace
 * Uso:   import postprocess_cpp as pp_cpp
 *        loc, exist = pp_cpp.postprocess(conv37, W1, b1, W2, b2)
 *        lanes      = pp_cpp.decode_lanes(loc, exist, single_lane_mode=False)
 */

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <openblas/cblas.h>

#include <algorithm>
#include <cmath>
#include <cstring>
#include <stdexcept>
#include <vector>

namespace py = pybind11;

// ── Constantes (espelham config.py) ──────────────────────────────────────────
static constexpr float QUANT_SCALE             = 0.271904319524765f;
static constexpr float QUANT_ZP               = 157.0f;
static constexpr int   NUM_CELL_ROW            = 100;
static constexpr int   NUM_ROW                 = 56;
static constexpr int   FULL_W                  = 820;
static constexpr int   FULL_H                  = 616;
static constexpr float EXIST_THRESHOLD         = 0.5f;
static constexpr float EXIST_THRESHOLD_TOP     = 0.8f;
static constexpr float EXIST_THRESHOLD_SINGLE  = 0.4f;
static constexpr int   LOCAL_WIDTH             = 14;

// ROW_ANCHOR = linspace(160, 710, 56) / 720.0
static float ROW_ANCHOR[NUM_ROW];
static bool  anchors_init = false;

static void ensure_anchors() {
    if (anchors_init) return;
    for (int i = 0; i < NUM_ROW; ++i)
        ROW_ANCHOR[i] = (160.0f + 550.0f * i / (NUM_ROW - 1)) / 720.0f;
    anchors_init = true;
}

// ── Softmax local ─────────────────────────────────────────────────────────────
static float softmax_local(const float* arr, int max_idx) {
    int lo = std::max(0, max_idx - LOCAL_WIDTH);
    int hi = std::min(NUM_CELL_ROW - 1, max_idx + LOCAL_WIDTH);

    float max_val = arr[lo];
    for (int i = lo + 1; i <= hi; ++i)
        if (arr[i] > max_val) max_val = arr[i];

    float sum = 0.0f, weighted = 0.0f;
    for (int i = lo; i <= hi; ++i) {
        float e = std::exp(arr[i] - max_val);
        sum      += e;
        weighted += e * i;
    }
    return weighted / sum + 0.5f;
}

// ── postprocess ───────────────────────────────────────────────────────────────
// Input:  conv37_uint8 (10, 25, 8) UINT8
//         W1 (2048, 2000)  b1 (2048,)
//         W2 (11424, 2048) b2 (11424,)
// Output: (loc_row_lanes12 (11200,), exist_row_lanes12 (224,))
std::pair<py::array_t<float>, py::array_t<float>>
postprocess(py::array_t<uint8_t, py::array::c_style | py::array::forcecast> conv37,
            py::array_t<float,   py::array::c_style | py::array::forcecast> W1,
            py::array_t<float,   py::array::c_style | py::array::forcecast> b1,
            py::array_t<float,   py::array::c_style | py::array::forcecast> W2,
            py::array_t<float,   py::array::c_style | py::array::forcecast> b2)
{
    if (conv37.size() != 2000)
        throw std::invalid_argument("conv37 deve ter 2000 elementos (10×25×8)");

    const int FC1_OUT = (int)W1.shape(0);   // 2048
    const int FC2_OUT = (int)W2.shape(0);   // 11424

    // Buffers de output
    auto loc_arr   = py::array_t<float>(11200);
    auto exist_arr = py::array_t<float>(224);

    {
        // Liberta o GIL durante todo o cálculo BLAS
        py::gil_scoped_release release;

        // Passo 1 — dequantização + transpose (10,25,8)→(8,10,25)→flatten(2000,)
        const uint8_t* src = conv37.data();
        float x[2000];
        // Original HWC: src[h*25*8 + w*8 + c]
        // Transposed CHW: x[c*250 + h*25 + w]
        for (int c = 0; c < 8; ++c)
            for (int h = 0; h < 10; ++h)
                for (int w = 0; w < 25; ++w)
                    x[c * 250 + h * 25 + w] =
                        (src[h * 200 + w * 8 + c] - QUANT_ZP) * QUANT_SCALE;

        // Passo 2 — FC1: h1 = W1 @ x + b1  →  ReLU
        std::vector<float> h1(FC1_OUT);
        std::memcpy(h1.data(), b1.data(), FC1_OUT * sizeof(float));
        cblas_sgemv(CblasRowMajor, CblasNoTrans,
                    FC1_OUT, 2000,
                    1.0f, W1.data(), 2000,
                    x, 1,
                    1.0f, h1.data(), 1);
        for (float& v : h1) if (v < 0.0f) v = 0.0f;

        // Passo 3 — FC2: out = W2 @ h1 + b2
        std::vector<float> out(FC2_OUT);
        std::memcpy(out.data(), b2.data(), FC2_OUT * sizeof(float));
        cblas_sgemv(CblasRowMajor, CblasNoTrans,
                    FC2_OUT, FC1_OUT,
                    1.0f, W2.data(), FC1_OUT,
                    h1.data(), 1,
                    1.0f, out.data(), 1);

        // Copia para os arrays de output
        std::memcpy(loc_arr.mutable_data(),   out.data(),         11200 * sizeof(float));
        std::memcpy(exist_arr.mutable_data(), out.data() + 11200,   224 * sizeof(float));
    }

    return {loc_arr, exist_arr};
}

// ── decode_lanes ──────────────────────────────────────────────────────────────
// Input:  loc_row_flat   (11200,) — reshape→(100, 56, 2)
//         exist_row_flat (224,)   — reshape→(2, 56, 2)
// Output: list[4] de list[(x,y)]  — lanes 0 e 3 vazias, 1 e 2 preenchidas
py::list decode_lanes(py::array_t<float, py::array::c_style | py::array::forcecast> loc_row_flat,
                      py::array_t<float, py::array::c_style | py::array::forcecast> exist_row_flat,
                      bool single_lane_mode = false)
{
    ensure_anchors();

    if (loc_row_flat.size() != 11200)
        throw std::invalid_argument("loc_row_flat deve ter 11200 elementos");
    if (exist_row_flat.size() != 224)
        throw std::invalid_argument("exist_row_flat deve ter 224 elementos");

    const float* loc   = loc_row_flat.data();    // (100, 56, 2)
    const float* exist = exist_row_flat.data();  // (2, 56, 2)

    // Softmax sobre dim=0 (cls) para cada (row, lane_i)
    // exist[cls * 56*2 + row * 2 + li]
    float row_score[NUM_ROW][2];
    for (int row = 0; row < NUM_ROW; ++row) {
        for (int li = 0; li < 2; ++li) {
            float e0 = exist[0 * NUM_ROW * 2 + row * 2 + li];
            float e1 = exist[1 * NUM_ROW * 2 + row * 2 + li];
            float m  = (e0 > e1) ? e0 : e1;
            float s0 = std::exp(e0 - m);
            float s1 = std::exp(e1 - m);
            row_score[row][li] = s1 / (s0 + s1);  // classe 1 = existe
        }
    }

    // Pré-extrai coluna loc para cada (row, li) — evita saltos de memória no inner loop
    // loc[cell * 56*2 + row * 2 + li]  →  col[cell]
    float col[NUM_CELL_ROW];

    py::list lanes(4);
    for (int i = 0; i < 4; ++i) lanes[i] = py::list();

    // ROW_LANE_LIST = [1, 2]: li=0 → lane_idx=1, li=1 → lane_idx=2
    for (int li = 0; li < 2; ++li) {
        int lane_idx = li + 1;
        py::list pts;

        for (int row = 0; row < NUM_ROW; ++row) {
            float thr;
            if (single_lane_mode)
                thr = (row <= 20) ? std::min(EXIST_THRESHOLD_SINGLE + 0.2f, 0.8f)
                                  : EXIST_THRESHOLD_SINGLE;
            else
                thr = (row > 20) ? EXIST_THRESHOLD : EXIST_THRESHOLD_TOP;

            if (row_score[row][li] < thr) continue;

            // Extrai coluna e encontra argmax
            int   max_idx = 0;
            float max_val = loc[0 * NUM_ROW * 2 + row * 2 + li];
            for (int cell = 0; cell < NUM_CELL_ROW; ++cell) {
                col[cell] = loc[cell * NUM_ROW * 2 + row * 2 + li];
                if (col[cell] > max_val) { max_val = col[cell]; max_idx = cell; }
            }

            float out_x = softmax_local(col, max_idx);
            int x_full  = (int)(out_x / (NUM_CELL_ROW - 1) * FULL_W);
            int y_full  = (int)(ROW_ANCHOR[row] * FULL_H);
            pts.append(py::make_tuple(x_full, y_full));
        }

        lanes[lane_idx] = pts;
    }

    return lanes;
}

// ── Binding ───────────────────────────────────────────────────────────────────
PYBIND11_MODULE(postprocess_cpp, m) {
    m.doc() = "SEAME UFLDv2 post-processing C++ (OpenBLAS, sem GIL durante BLAS)";
    ensure_anchors();

    m.def("postprocess", &postprocess,
          py::arg("conv37_uint8"), py::arg("W1"), py::arg("b1"),
          py::arg("W2"), py::arg("b2"),
          "FC pipeline: dequant + FC1 + ReLU + FC2 → (loc_row, exist_row)");

    m.def("decode_lanes", &decode_lanes,
          py::arg("loc_row_flat"), py::arg("exist_row_flat"),
          py::arg("single_lane_mode") = false,
          "Decode tensors → list[4] de list[(x,y)]");
}
