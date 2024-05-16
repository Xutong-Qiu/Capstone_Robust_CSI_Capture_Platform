hex_data = """
0000   77 01 00 02 25 08 0f 02 00 df dc df dd 00 00 00
0010   04 00 06 45 72 0e 6a e0 00 00 00 0c 00 00 00 00
0020   00 00 00 00 ad de d0 07 00 00 00 00 00 00 00 00
0030   00 00 00 00 54 00 64 00 00 01 02 03 04 05 00 0a
0040   0b 0c 0d 0e 00 00 00 00 00 00 00 00 00 00 01 00
0050   00 00 4e 3d 74 00 96 00 00 01 02 03 04 05 b4 00
0060   00 00 c0 01 00 0a 0b 0c 0d 0e cb 99 00 00 00 00
0070   04 01 00 00 00 00 0c 00 4e 3d 00 00 eb eb eb 2d
0080   00 00 00 00 80 00 00 00 ff ff ff ff ff ff a0 36
0090   bc b2 de 14 a0 36 bc b2 de 14 70 bb 3b 30 6e 46
00a0   57 05 00 00 64 00 11 11 00 10 4e 45 53 4c 2d 4a
00b0   43 41 53 2d 32 2d 35 47 48 7a 01 08 8c 12 18 24
00c0   b0 48 60 6c 05 04 00 01 00 00 07 0c 55 53 20 24
00d0   08 18 64 0c 18 95 05 1e 20 01 00 23 02 12 00 30
00e0   14 01 00 00 0f ac 04 01 00 00 0f ac 04 01 00 00
00f0   0f ac 02 0c 00 0b 05 00 00 03 00 00 46 05 32 00
0100   00 00 00 3b 10 04 01 02 03 04 05 16 17 18 19 1b
0110   1c 1d 1e 80 81 2d 1a ad 01 17 ff ff ff ff 00 00
0120   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
0130   00 3d 16 68 00 00 00 00 00 00 00 00 00 00 00 00
0140   00 00 00 00 00 00 00 00 00 7f 09 04 00 08 80 01
0150   00 00 c0 01 bf 0c b1 79 8b 0f aa ff 00 00 aa ff
0160   00 20 c0 05 00 68 00 00 00 c3 02 00 25 dd 1d 00
0170   50 f2 04 10 4a 00 01 10 10 44 00 01 02 10 3c 00
0180   01 03 10 49 00 06 00 37 2a 00 01 20 dd 31 f8 32
0190   e4 01 01 01 02 01 00 03 14 39 d3 b5 3a 0a 46 e5
01a0   0c 8e f0 df 1b 87 da 8a c3 65 f0 a3 40 07 04 65
01b0   f0 a3 40 12 04 10 14 00 00 13 01 01 15 01 00 dd
01c0   05 00 90 4c 04 17 dd 09 00 10 18 02 00 00 9c 00
01d0   00 dd 18 00 50 f2 02 01 01 84 00 03 a4 00 00 27
01e0   a4 00 00 42 43 5e 00 62 32 2f 00 6c 02 7f 00 0c
01f0   67 4c 42
"""

# Remove line numbers
lines = hex_data.strip().split('\n')
hex_numbers = [line.split(maxsplit=1)[1] for line in lines]

# Create a single string of hex numbers
single_string = ' '.join(hex_numbers)

# Split into individual hex numbers
hex_values = single_string.split()

# Format with '0x' prefix and commas
formatted_hex = ', '.join(f"0x{hex_val}" for hex_val in hex_values)

print(formatted_hex)
