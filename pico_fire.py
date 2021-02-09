import utime, uarray, random
import picodisplay as display

colorScale = [
    display.create_pen(0x07,0x07,0x07),
    display.create_pen(0x1F,0x07,0x07),
    display.create_pen(0x2F,0x0F,0x07),
    display.create_pen(0x47,0x0F,0x07),
    display.create_pen(0x57,0x17,0x07),
    display.create_pen(0x67,0x1F,0x07),
    display.create_pen(0x77,0x1F,0x07),
    display.create_pen(0x8F,0x27,0x07),
    display.create_pen(0x9F,0x2F,0x07),
    display.create_pen(0xAF,0x3F,0x07),
    display.create_pen(0xBF,0x47,0x07),
    display.create_pen(0xC7,0x47,0x07),
    display.create_pen(0xDF,0x4F,0x07),
    display.create_pen(0xDF,0x57,0x07),
    display.create_pen(0xDF,0x57,0x07),
    display.create_pen(0xD7,0x5F,0x07),
    display.create_pen(0xD7,0x5F,0x07),
    display.create_pen(0xD7,0x67,0x0F),
    display.create_pen(0xCF,0x6F,0x0F),
    display.create_pen(0xCF,0x77,0x0F),
    display.create_pen(0xCF,0x7F,0x0F),
    display.create_pen(0xCF,0x87,0x17),
    display.create_pen(0xC7,0x87,0x17),
    display.create_pen(0xC7,0x8F,0x17),
    display.create_pen(0xC7,0x97,0x1F),
    display.create_pen(0xBF,0x9F,0x1F),
    display.create_pen(0xBF,0x9F,0x1F),
    display.create_pen(0xBF,0xA7,0x27),
    display.create_pen(0xBF,0xA7,0x27),
    display.create_pen(0xBF,0xAF,0x2F),
    display.create_pen(0xB7,0xAF,0x2F),
    display.create_pen(0xB7,0xB7,0x2F),
    display.create_pen(0xB7,0xB7,0x37),
    display.create_pen(0xCF,0xCF,0x6F),
    display.create_pen(0xDF,0xDF,0x9F),
    display.create_pen(0xEF,0xEF,0xC7),
    display.create_pen(0xFF,0xFF,0xFF)
];

width = display.get_width()
height = display.get_height()

display_buffer = bytearray(width * height * 2)  # 2-bytes per pixel (RGB565)
display.init(display_buffer)

display.set_backlight(1.0)

class DoomFire:
    def __init__(self):
        # Store the colorScale index for each pixel. There are only 36 colors, so 1 byte per pixel is enough.
        self.fire = bytearray(width * height)

        # Initialize all the pixels in the bottom row with the last index.
        last_row = (height - 1) * width
        for x in range(0, width):
            self.fire[last_row + x] = 35

    def update(self):
        for y in range(0, height):
            # We precompute the rows for a small performance gain.
            row = y * width

            # For each pixel in each row, we calculate the colours that will be
            # rendered on the previous row, on the next call to update.
            next_row = (y - 1) * width
            for x in range(0, width):
                color = self.fire[row + x]
                pen = colorScale[color]
                if y > 0:
                    new_x = x

                    # We're already at the first color index and can't move further.
                    if color > 0:
                        rand = random.randint(0, 3)

                        # Maybe move to the next colour.
                        color = color - (rand & 1)

                        # Spread the fire left and right.
                        new_x = new_x + rand - 1
                    self.fire[next_row + new_x] = color

                display.set_pen(pen)
                display.pixel(x, y)
        display.update()

doom_fire = DoomFire()
while True:
    start = utime.ticks_ms()
    doom_fire.update()
    print(utime.ticks_ms() - start)
