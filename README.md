# Autotune

A simple autotune plugin for Windows, MacOS and Linux.

## Installation

Download the appropiate binary from the releases page and copy it to the appropiate plugin search path of your system.

## Usage

The plugin can be set up as an insert effect on an audio track and is realtime capable. 
It has two parameters:

1.  **Major Key:** the major key to tune to. To tune to a minor key, please transpose.
2.  **Minimum Period detection probability:** how closely the signal has to match it's delayed version so that the delay is considered a valid period. 
For a more detailed explanation, see below. 
Lower values make the algorithm more robust against noise, but increase the probability of misdetections.
A value of 1 means that the signal has to exactly match it's delayed version (with non-artificial signals the autotune will be off). 
A value of 0 means that everything is considered a valid period, so the autotune will not tune properly, but glitch heavily.

## Potential improvements

-   **Integration time parameter:** it would be good if the algorithm would not tune straight to the goal frequency after a transition, but if it's strength would increase over time, resulting in smooth transitions after tone changes and therefore a more natural sound.
-   **Maximum strength parameter:** similarly, it would be beneficial if the maximum strength could be reduced to leave some natural frequency variation in, also leading to a more natural sound.
-   **FFT library:** currently, KissFFT is used as an FFt library. It is a great library that is easy to use and implement, but there are faster libraries available. The FFT library could be switched to increase performance.
-   **Intrinsics:** for the other calculations, intrinsics like SSE or AVX could be use to increase performance.
-   **Pitch detection:** the pitch detection algorithm sometimes detects a wrong frequency that is up to 20% different from the correct frequency. This only happens for one or two blocks in a row and therefore can be worked around (see [below](#Algorithm-documentation). It works, but it is not really pretty.
-   **quality for bigger block sizes**. Currently, the pitch is only detected for a single block. It would be beneficial to detect pitch multiple times per block to improve quality for higher block sizes.
-   **Integration Tests** Currently, the integration tests fail on some systems. This is probably due to differences in floating point calculation between systems.


## Algorithm documentation

For pitch shifting, the efficient PSOLA (pitch-synchronous overlap-add) method is used.
Please see [this great explanation](https://speechprocessingbook.aalto.fi/Representations/Pitch-Synchoronous_Overlap-Add_PSOLA.html) by Aalto university to understand how it works.

For pitch detection, an extended version of the NSDF (normalised square difference function) method suggested by [McLeod et al.](https://www.cs.otago.ac.nz/graphics/Geoff/tartini/papers/A_Smarter_Way_to_Find_Pitch.pdf) is used. 
The NSDF is essentially a normalised version of the autocorrelation function and describes the similarity between a signal and a delayed version of itself.
A value of 1 that the overlapping parts match exactly, a value of 0 means that they do not match at all.

The pitch detection works in multiple steps:

1.  Calculate the NSDF of the latest signal part.
2.  All local maxima of the NSDF that are higher than the minimum period detection probability set by the user are detected.
3.  For all $x \in \{0.05, 0.1, ..., 1\}$, the first peak that from the peaks in step 1 that is higher than $x \cdot \max{(\mathrm{NSDF})}$ is selected (credits to [Sevagh](https://github.com/sevagh/pitch-detection/tree/master/misc/probabilistic-mcleod)).
4.  From the 20 peak values calculated in step 2, the most frequent one is saved for the next step.
5.  The NSDF is truncated at the beginning by the period length estimated in step 3. Steps 2-4 are then applied recursively again. Out of all resulting detected periods, the mean value is considered to be the period of the signal.

Unfortunately, the algorithm occasionally detects a frequency that is up to 20% different from the correct frequency. 
Fortunately, this happens only one or two blocks in a row and can therefore be corrected.
If the currently detected frequency is more than 1% different from the last one, it is probably a misdetection and overwritten by the last detected method.
Up to three misdetections in a row are allowed, after which the frequency will be updated.
If no valid period is detected, the current block probably contains a consonant. It is then not tuned and the misdetection counter is reset.
