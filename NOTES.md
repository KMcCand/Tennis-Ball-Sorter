Initial Notes:

- We realized we had to convert 5 volts to 3.3 volts for the Arduino to talk to the TFmini plus using [this converter](https://www.amazon.com/gp/product/B07LG646VS/ref=ppx_yo_dt_b_asin_title_o01_s00?ie=UTF8&psc=1).
- [This is the Lidar module](https://www.amazon.com/gp/product/B07L68KM32/ref=ppx_yo_dt_b_asin_title_o08_s01?ie=UTF8&psc=1) we're using and [this is its data sheet](https://cdn.sparkfun.com/assets/1/4/2/1/9/TFmini_Plus_A02_Product_Manual_EN.pdf).
- [This is the four-inch inner diameter, four feet in length Lexan tube we are using](https://www.amazon.com/gp/product/B00FDWX2O6/ref=ppx_yo_dt_b_asin_title_o01_s00?ie=UTF8&psc=1) and [this is the three-inch inner diameter, four feet in length Lexan tube we are using](https://www.amazon.com/gp/product/B00FDWXNIG/ref=ppx_yo_dt_b_asin_title_o01_s00?ie=UTF8&psc=1).
- We used [this Arduino library](https://github.com/budryerson/TFMini-Plus) to talk to the TFmini plus from Arduino
- We modified the sample program from the Github repository to print the distance with millisecond time stamps, and noticed something unexpected: the distance could not jump from far to close quickly, as if it has a low-pass filter built in.
- We ran an experiment comparing closed and open bottom of the tube, bouncing a dead ball. Recorded [here](https://docs.google.com/document/d/1NsJhDZJKyt4JIkerm2EDqj8B_bCkIEIaSGx8xBFSB3Y/edit?usp=sharing).
