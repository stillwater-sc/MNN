//
//  DeconvolutionTest.cpp
//  MNNTests
//
//  Created by MNN on 2019/12/31.
//  Copyright © 2018, Alibaba Group Holding Limited
//

#include <MNN/expr/Expr.hpp>
#include <MNN/expr/ExprCreator.hpp>
#include <string>
#include <vector>
#include "MNNTestSuite.h"
#include "TestUtils.h"
using namespace std;
using namespace MNN;
using namespace MNN::Express;

class DeconvolutionCommonTest : public MNNTestCase {
public:
    virtual ~DeconvolutionCommonTest() = default;

protected:
    static bool test(MNNForwardType type, const std::string& device_name, const std::string& test_op_name,
                    vector<float>& inputData, vector<float>& weightData, vector<float>& biasData, vector<float>& rightOutData,
                    int batch, int ic, int oc, int ih, int iw, PadMode mode, int pad_h, int pad_w, int kh,
                    int kw, int stride, int dilation, int group) {
        std::map<PadMode, Express::PaddingMode> padMap = {
            {PadMode_CAFFE, CAFFE}, {PadMode_VALID, VALID}, {PadMode_SAME, SAME}};
        auto input = _Input({batch, ic, ih, iw}, NCHW, halide_type_of<float>());
        ::memcpy(input->writeMap<float>(), inputData.data(), inputData.size() * sizeof(float));
        auto output = _Deconv(std::move(weightData), std::move(biasData), input, {ic, oc}, {kw, kh}, padMap[mode],
                              {stride, stride}, {dilation, dilation}, group, {pad_w, pad_h}, false, false);

        // difference below 0.5% relative error is considered correct.
        auto outputPtr = output->readMap<float>();
        if (!checkVectorByRelativeError<float>(outputPtr, rightOutData.data(), rightOutData.size(), 0.005)) {
            MNN_ERROR("%s(%s) test failed!\n", test_op_name.c_str(), device_name.c_str());
            return false;
        }
        return true;
    }
};

class DeconvolutionTest : public DeconvolutionCommonTest {
public:
    virtual ~DeconvolutionTest() = default;
    virtual bool run(int precision) {
        MNN_PRINT("beigin testcase 0\n");

        {
            std::vector<float> data_a = {// channel 0
                                         1.0, 2.0, 4.0, 5.0,
                                         // channel 1
                                         1.1, 2.1, 4.1, 5.1,
                                         // channel 2
                                         1.2, 2.2, 4.2, 5.2};

            std::vector<float> weight = {
                // output channel0
                // input channel0
                1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                // input channel1
                2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0,
                // input channel2
                1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,

                // output channel1
                // input channel0
                2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0,
                // input channel1
                1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                // input channel2
                2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0,
            };
            std::vector<float> bias   = {0.0, 0.0};
            std::vector<float> data_c = {3.3,  3.3,  9.6,  6.3,  6.3,  3.3,  3.3,  9.6,  6.3,  6.3,  15.6, 15.6, 37.2,
                                         21.6, 21.6, 12.3, 12.3, 27.6, 15.3, 15.3, 12.3, 12.3, 27.6, 15.3, 15.3,

                                         6.6,  6.6,  19.2, 12.6, 12.6, 6.6,  6.6,  19.2, 12.6, 12.6, 31.2, 31.2, 74.4,
                                         43.2, 43.2, 24.6, 24.6, 55.2, 30.6, 30.6, 24.6, 24.6, 55.2, 30.6, 30.6};

            int ic = 3, oc = 2;
            int kw = 3, kh = 3, ih = 2, iw = 2;
            int stride = 2, dilation = 1;
            int group = 1, batch = 1;
            int pad_w = 0, pad_h = 0;

            bool succ = DeconvolutionCommonTest::test(MNN_FORWARD_CPU, "CPU", "DeconvolutionTest0", data_a, weight, bias, data_c,
                                                      batch, ic, oc, ih, iw, PadMode_VALID, pad_h, pad_w, kh, kw,
                                                      stride, dilation, group);
            if (!succ) {
                return false;
            }
        }

        MNN_PRINT("beigin testcase 1\n");
        {
            std::vector<float> data_a = {// channel 0
                                         1.0, 2.0, 4.0, 5.0,
                                         // channel 1
                                         1.1, 2.1, 4.1, 5.1,
                                         // channel 2
                                         1.2, 2.2, 4.2, 5.2};

            std::vector<float> weight = {
                // output channel0
                // input channel0
                1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                // input channel1
                2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0,
                // input channel2
                1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,

                // output channel1
                // input channel0
                2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0,
                // input channel1
                1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                // input channel2
                2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0,
            };
            std::vector<float> bias   = {1.0, 2.0};
            std::vector<float> data_c = {
                4.3, 10.6, 10.6, 7.3,  16.6, 38.2, 38.2, 22.6, 16.6, 38.2, 38.2, 22.6, 13.3, 28.6, 28.6, 16.3,

                8.6, 21.2, 21.2, 14.6, 33.2, 76.4, 76.4, 45.2, 33.2, 76.4, 76.4, 45.2, 26.6, 57.2, 57.2, 32.6,
            };
            int ic = 3, oc = 2;
            int kw = 4, kh = 4, ih = 2, iw = 2;
            int stride = 2, dilation = 1;
            int group = 1, batch = 1;
            int pad_w = 1, pad_h = 1;

            bool succ = DeconvolutionCommonTest::test(MNN_FORWARD_CPU, "CPU", "Deconv", data_a, weight, bias, data_c,
                                                      batch, ic, oc, ih, iw, PadMode_VALID, pad_h, pad_w, kh, kw,
                                                      stride, dilation, group);
            if (!succ) {
                return false;
            }
        }

        MNN_PRINT("beigin testcase 2\n");
        {
            std::vector<float> data_a = {// channel 0
                                         1.0, 2.0, 4.0, 5.0,
                                         // channel 1
                                         1.1, 2.1, 4.1, 5.1,
                                         // channel 2
                                         1.2, 2.2, 4.2, 5.2};

            std::vector<float> weight = {
                // output channel0
                // input channel0
                1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                // input channel1
                2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0,
                // input channel2
                1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,

                // output channel1
                // input channel0
                2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0,
                // input channel1
                1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                // input channel2
                2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0,
            };
            std::vector<float> bias   = {0.0, 0.0};
            std::vector<float> data_c = {3.3,  3.3,  9.6,  6.3,  3.3,  3.3,  9.6,  6.3, 15.6, 15.6, 37.2,
                                         21.6, 12.3, 12.3, 27.6, 15.3,

                                         6.6,  6.6,  19.2, 12.6, 6.6,  6.6,  19.2, 12.6, 31.2, 31.2, 74.4,
                                         43.2, 24.6, 24.6, 55.2, 30.6};
            int ic = 3, oc = 2;
            int kw = 3, kh = 3, ih = 2, iw = 2;
            int stride = 2, dilation = 1;
            int group = 1, batch = 1;
            int pad_w = 0, pad_h = 0;

            bool succ = DeconvolutionCommonTest::test(MNN_FORWARD_CPU, "CPU", "Deconv", data_a, weight, bias, data_c,
                                                      batch, ic, oc, ih, iw, PadMode_SAME, pad_h, pad_w, kh, kw,
                                                      stride, dilation, group);
            if (!succ) {
                return false;
            }
        }

        return true;
    }
};
MNNTestSuiteRegister(DeconvolutionTest, "op/Deconvolution");

