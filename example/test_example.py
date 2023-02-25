def test_example(dut):
    dut.expect_exact("Example ready")
    dut.write(b"$GPRMC,170058.89,V,3554.928,N,08002.496,W,9.4,1.57,050521,,E*41\r\n")
    dut.expect_exact(
        """
GPRMC sentence
Longitude:
  Degrees: 80
  Minutes: 2.496000
  Cardinal: W
Latitude:
  Degrees: 35
  Minutes: 54.928000
  Cardinal: N
Date & Time: 05 May 17:00:58 2021
Speed, in Knots: 9.400000
Track, in degrees: 1.570000
Magnetic Variation:
  Degrees: 0.000000
  Cardinal: E
Adjusted Track (heading): 1.570000
""".strip(),
        timeout=5,
    )
