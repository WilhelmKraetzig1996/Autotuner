#include <array>

#include <gtest/gtest.h>

#include "autotuner/AudioBuffer.h"
#include "autotuner/CircularAudioBuffer.h"
#include "autotuner/NormalizedSquareDifferenceCalculator.h"


namespace autotuner::backend
{
    using namespace testing;

    namespace
    {
        constexpr size_t SIGNAL_LEN = 32;
    }

    class IntegrationTest_NormalizedSquareDifferenceCalculator : public Test
    {
    protected:
        void SetUp() override
        {
            inputValues_ = {-0.16595599f, 0.44064899f, -0.99977125f, -0.39533485f, -0.70648822f, -0.81532281f, -0.62747958f, -0.30887855f, -0.20646505f, 0.07763347f, -0.16161097f, 0.370439f, -0.5910955f, 0.75623487f, -0.94522481f, 0.34093502f, -0.1653904f, 0.11737966f, -0.71922612f, -0.60379702f, 0.60148914f, 0.93652315f, -0.37315164f, 0.38464523f, 0.7527783f, 0.78921333f, -0.82991158f, -0.92189043f, -0.66033916f, 0.75628501f, -0.80330633f, -0.15778475f};
            akfValues_ = {0.31810365f, 1.38984926f, -1.02542716f, 3.80276628f, -0.9831867f, -2.36123823f, -2.50977425f, 2.81661754f, -0.25431581f, 0.77609274f, -1.54593241f, 3.54484096f, -0.68030072f, 1.82711191f, -1.87173682f, 1.329986f, -2.03019898f, -1.04223919f, -1.73537813f, 0.43049546f, -0.29350946f, 0.83912612f, 0.80277904f, 1.8197152f, 0.9057145f, 0.78887296f, -0.46504849f, 1.30834385f, -0.32173749f, 0.06378581f, 0.02618532f};
            nsdfValues_ = {0.02650259f, 0.11999043f, -0.09497117f, 0.36213105f, -0.10005387f, -0.25806386f, -0.29043179f, 0.33892329f, -0.03095675f, 0.09531308f, -0.20100628f, 0.47636374f, -0.09602696f, 0.27937972f, -0.30753413f, 0.22113074f, -0.34163005f, -0.18987541f, -0.35097282f, 0.09384102f, -0.06766023f, 0.21591382f, 0.21049547f, 0.48937212f, 0.26737522f, 0.27399656f, -0.21115702f, 0.85631406f, -0.26121209f, 0.14303194f, 0.99872669f};

            std::copy(inputValues_.begin(), inputValues_.end(), &input_[0]);
            std::copy(akfValues_.begin(), akfValues_.end(), &akfRef_[0]);
            std::copy(nsdfValues_.begin(), nsdfValues_.end(), &nsdfRef_[0]);
        }

        std::array<float, SIGNAL_LEN> inputValues_;
        std::array<float, SIGNAL_LEN - 1> akfValues_;
        std::array<float, SIGNAL_LEN - 1> nsdfValues_;

        CircularAudioBuffer input_ {SIGNAL_LEN};
        AudioBuffer akfRef_ {SIGNAL_LEN - 1};
        AudioBuffer nsdfRef_ {SIGNAL_LEN - 1};
    };

    TEST_F(IntegrationTest_NormalizedSquareDifferenceCalculator, process)
    {


        NormalizedSquareDifferenceCalculator calculator(SIGNAL_LEN, SIGNAL_LEN);
        AudioBuffer nsdf = calculator.process(input_);
        const AudioBuffer& akf = calculator.akf();

        for (size_t i = 0; i < SIGNAL_LEN - 1; ++i)
        {
            EXPECT_NEAR(nsdf[i], nsdfRef_[i], 7e-5) << "error at sample " << i;
            EXPECT_NEAR(akf[i + 1], akfRef_[i], 7e-5) << "error at sample " << i;
        }
    }
}
