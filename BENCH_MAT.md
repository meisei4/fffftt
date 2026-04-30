Key:
`fast math` 
  - `OFF` = comment out [Makefile:15](./Makefile#L15) (for fftw-dc only)

`profiling stuff`
  - `OFF` = uncomment [Makefile:18](./Makefile#L18)

`trace logging`
  - `INFO` = comment out [cool_dc.c:18](./src/cool_dc.c#L18), [fftw_dc.c:23](./src/fftw_dc.c#L23), [sh4zam_butterfly.c:19](./src/sh4zam_butterfly.c#L19)

`period`
  - `2048` = default build, e.g. `make audio-only-mp3-dc`
  - `1024` = override build, e.g. `make audio-only-mp3-dc AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES=1024`
  - same pattern applies to other dc targets e.g. `make cool-dc AUDIO_DEVICE_PERIOD_SIZE_IN_FRAMES=1024`

```
+------------------------------------------------------------------------------------------------------------+
| COOL-DC                                                                                                    |
+----------+--------+-----------+---------+-------+-------------+-------------+--------+-------------+-------+
| platform | period | fast math | profile | trace | buffer size | min ms      | max ms | avg ms      | audio |
+----------+--------+-----------+---------+-------+-------------+-------------+--------+-------------+-------+
| FLYCAST  | 2048   | ON        | ON      | INFO  | 4096        | 0.892       | 2.820  | 0.893-1.069 | GOOD  |
| FLYCAST  | 1024   | ON        | ON      | INFO  | 2048        | 0.892       | 1.877  | 0.894-0.980 | GOOD  |
| REAL_HW  | 2048   | ON        | ON      | INFO  | 4096        | 0.746-0.747 | 4.100  | 0.748-0.930 | BAD   |
| REAL_HW  | 1024   | ON        | ON      | INFO  | 2048        | 0.746       | 2.568  | 0.751-0.874 | BAD   |
| REAL_HW  | 2048   | ON        | ON      | WARN  | 4096        | N/A         | N/A    | N/A         | GOOD  |
| REAL_HW  | 2048   | ON        | OFF     | WARN  | 4096        | N/A         | N/A    | N/A         | GOOD  |
| REAL_HW  | 1024   | ON        | ON      | WARN  | 2048        | N/A         | N/A    | N/A         | MIXED |
| REAL_HW  | 1024   | ON        | OFF     | WARN  | 2048        | N/A         | N/A    | N/A         | MIXED |
+----------+--------+-----------+---------+-------+-------------+-------------+--------+-------------+-------+

+------------------------------------------------------------------------------------------------------------+
| FFTW-DC                                                                                                    |
+----------+--------+-----------+---------+-------+-------------+-------------+--------+-------------+-------+
| platform | period | fast math | profile | trace | buffer size | min ms      | max ms | avg ms      | audio |
+----------+--------+-----------+---------+-------+-------------+-------------+--------+-------------+-------+
| FLYCAST  | 2048   | OFF       | ON      | INFO  | 4096        | 0.685       | 3.190  | 0.687-0.762 | GOOD  |
| FLYCAST  | 2048   | ON        | ON      | INFO  | 4096        | 0.844       | 2.769  | 0.847-0.944 | GOOD  |
| FLYCAST  | 1024   | OFF       | ON      | INFO  | 2048        | 0.685       | 1.669  | 0.703-0.758 | GOOD  |
| FLYCAST  | 1024   | ON        | ON      | INFO  | 2048        | 0.844       | 1.801  | 0.846-0.916 | GOOD  |
| REAL_HW  | 2048   | OFF       | ON      | INFO  | 4096        | 1.879-1.881 | 5.286  | 1.892-2.232 | BAD   |
| REAL_HW  | 2048   | ON        | ON      | INFO  | 4096        | 2.274-2.292 | 5.650  | 2.289-2.717 | BAD   |
| REAL_HW  | 1024   | OFF       | ON      | INFO  | 2048        | 1.879-1.885 | 3.726  | 1.891-2.122 | BAD   |
| REAL_HW  | 1024   | ON        | ON      | INFO  | 2048        | 2.274-2.296 | 4.125  | 2.296-2.536 | BAD   |
| REAL_HW  | 2048   | OFF       | ON      | WARN  | 4096        | N/A         | N/A    | N/A         | GOOD  |
| REAL_HW  | 2048   | OFF       | OFF     | WARN  | 4096        | N/A         | N/A    | N/A         | GOOD  |
| REAL_HW  | 1024   | OFF       | ON      | WARN  | 2048        | N/A         | N/A    | N/A         | MIXED |
| REAL_HW  | 1024   | OFF       | OFF     | WARN  | 2048        | N/A         | N/A    | N/A         | MIXED*|
+----------+--------+-----------+---------+-------+-------------+-------------+--------+-------------+-------+

+------------------------------------------------------------------------------------------------------------+
| SH4ZAM-BUTTERFLY                                                                                           |
+----------+--------+-----------+---------+-------+-------------+-------------+--------+-------------+-------+
| platform | period | fast math | profile | trace | buffer size | min ms      | max ms | avg ms      | audio |
+----------+--------+-----------+---------+-------+-------------+-------------+--------+-------------+-------+
| FLYCAST  | 2048   | ON        | ON      | INFO  | 4096        | 0.614       | 3.121  | 0.615-0.747 | GOOD  |
| FLYCAST  | 1024   | ON        | ON      | INFO  | 2048        | 0.614       | 1.597  | 0.614-0.712 | GOOD  |
| REAL_HW  | 2048   | ON        | ON      | INFO  | 4096        | 0.711-0.728 | 4.096  | 0.713-0.836 | BAD   |
| REAL_HW  | 1024   | ON        | ON      | INFO  | 2048        | 0.711-0.712 | 2.531  | 0.715-0.864 | BAD   |
| REAL_HW  | 2048   | ON        | ON      | WARN  | 4096        | N/A         | N/A    | N/A         | GOOD  |
| REAL_HW  | 2048   | ON        | OFF     | WARN  | 4096        | N/A         | N/A    | N/A         | GOOD  |
| REAL_HW  | 1024   | ON        | ON      | WARN  | 2048        | N/A         | N/A    | N/A         | MIXED*|
| REAL_HW  | 1024   | ON        | OFF     | WARN  | 2048        | N/A         | N/A    | N/A         | MIXED*|
+----------+--------+-----------+---------+-------+-------------+-------------+--------+-------------+-------+
```
> * `REAL_HW / 1024 / ON/OFF / WARN / sh4zam-butterfly`: some crackles/skips, but somehow noticebly less than `cool-dc` and `fftw-dc`??? MAYBE???? very hard to tell
```
+------------------------------------------------------------------------------------------------------------+
| AUDIO-ONLY-DC                                                                                              |
+------------+--------+----------+---------+------------------------------------------------------+----------+
| platform   | mode   | period   | trace   | ffprobe                                              | audio    |
+------------+--------+----------+---------+------------------------------------------------------+----------+
| FLYCAST    | WAV    | 2048     | INFO    | wav/22050/mono/s16le                                 | GOOD     |
| FLYCAST    | WAV    | 1024     | INFO    | wav/22050/mono/s16le                                 | GOOD     |
| FLYCAST    | MP3    | 2048     | INFO    | mp3/44100/stereo/128k                                | GOOD     |
| FLYCAST    | MP3    | 1024     | INFO    | mp3/44100/stereo/128k                                | BAD      |
| FLYCAST    | WAV    | 2048     | WARN    | wav/22050/mono/s16le                                 | GOOD     |
| FLYCAST    | WAV    | 1024     | WARN    | wav/22050/mono/s16le                                 | GOOD     |
| FLYCAST    | MP3    | 2048     | WARN    | mp3/44100/stereo/128k                                | GOOD     |
| FLYCAST    | MP3    | 1024     | WARN    | mp3/44100/stereo/128k                                | BAD      |
| REAL_HW    | MP3    | 2048     | WARN    | mp3/44100/stereo/128k                                | TODO*    |
| REAL_HW    | MP3    | 1024     | WARN    | mp3/44100/stereo/128k                                | TODO*    |
| REAL_HW    | MP3    | 2048     | INFO    | mp3/44100/stereo/128k                                | GOOD     |
| REAL_HW    | MP3    | 1024     | INFO    | mp3/44100/stereo/128k                                | BAD+SLOW |
| REAL_HW    | WAV    | 2048     | WARN    | wav/22050/mono/s16le                                 | TODO*    |
| REAL_HW    | WAV    | 2048     | INFO    | wav/22050/mono/s16le                                 | GOOD     |
| REAL_HW    | WAV    | 1024     | WARN    | wav/22050/mono/s16le                                 | TODO*    |
| REAL_HW    | WAV    | 1024     | INFO    | wav/22050/mono/s16le                                 | GOOD     |
+------------+--------+----------+---------+------------------------------------------------------+----------+
```
> *TODO: nothing to even log here, but set up to allow the toggle, untested 
