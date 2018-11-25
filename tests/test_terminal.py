import cffi
import unittest


class TestS2HellTerminal(unittest.TestCase):
    def setUp(self):
        self.ffi = cffi.FFI()
        with open("output/s2hell.h.preprocessed") as f:
            self.ffi.cdef(f.read())

        self.lib = self.ffi.dlopen("output/x86/s2hell.so")
        # mandatory to prevent side effect !
        self.lib.s2hell_clear_line()

    def test_add_character(self):
        result = self.lib.s2hell_add_character(self.ffi.cast('uint8_t', 65))

        self.assertEqual(result, 0)

    def test_get_line_size(self):
        self.lib.s2hell_add_character(self.ffi.cast('uint8_t', 65))
        result = self.lib.s2hell_get_line_size()

        self.assertEqual(result, 1)

    def test_get_line_buffer(self):
        self.lib.s2hell_add_character(self.ffi.cast('uint8_t', 65))
        size = self.lib.s2hell_get_line_size()
        buffer = self.lib.s2hell_get_line_buffer()
        result = bytes(buffer[0:size])

        self.assertEqual(result, b'A')

    def test_add_character_buffer_overflow(self):
        for i in range(81):
            result = self.lib.s2hell_add_character(self.ffi.cast('uint8_t', 65))

        self.assertEqual(result, 255)

    def test_add_character_not_ascii_1(self):
        # test char under 32
        result = self.lib.s2hell_add_character(self.ffi.cast('uint8_t', 12))

        self.assertEqual(result, 255)

    def test_add_character_not_ascii_2(self):
        # test char over 126
        result = self.lib.s2hell_add_character(self.ffi.cast('uint8_t', 192))

        self.assertEqual(result, 255)

    def test_add_char_and_cursor_1(self):
        [self.lib.s2hell_add_character(self.ffi.cast('uint8_t', int(i))) for i in b'teaz']
        self.lib.s2hell_move_cursor_backward(2)
        [self.lib.s2hell_add_character(self.ffi.cast('uint8_t', i)) for i in b'sts']

        size = self.lib.s2hell_get_line_size()
        buffer = self.lib.s2hell_get_line_buffer()
        result = bytes(buffer[0:size])

        self.assertEqual(result, b'tests')

    def test_add_char_and_cursor_2(self):
        [self.lib.s2hell_add_character(
            self.ffi.cast('uint8_t', int(i))) for i in b'teaz']
        self.lib.s2hell_move_cursor_backward(100)
        self.lib.s2hell_move_cursor_forward(2)
        [self.lib.s2hell_add_character(
            self.ffi.cast('uint8_t', i)) for i in b'sts']

        size = self.lib.s2hell_get_line_size()
        buffer = self.lib.s2hell_get_line_buffer()
        result = bytes(buffer[0:size])

        self.assertEqual(result, b'tests')

    def test_backspace_1(self):
        [self.lib.s2hell_add_character(
            self.ffi.cast('uint8_t', int(i))) for i in b'teaz']
        
        self.lib.s2hell_backspace()


        size = self.lib.s2hell_get_line_size()
        buffer = self.lib.s2hell_get_line_buffer()
        result = bytes(buffer[0:size])

        self.assertEqual(result, b'tea')

    def test_backspace_2(self):
        [self.lib.s2hell_add_character(
            self.ffi.cast('uint8_t', int(i))) for i in b'teaz']

        self.lib.s2hell_move_cursor_backward(1)
        self.lib.s2hell_backspace()

        size = self.lib.s2hell_get_line_size()
        buffer = self.lib.s2hell_get_line_buffer()
        result = bytes(buffer[0:size])

        self.assertEqual(result, b'tez')

    def test_backspace_3(self):
        [self.lib.s2hell_add_character(
            self.ffi.cast('uint8_t', int(i))) for i in b'teaz']

        self.lib.s2hell_move_cursor_backward(10)
        self.lib.s2hell_backspace()

        size = self.lib.s2hell_get_line_size()
        buffer = self.lib.s2hell_get_line_buffer()
        result = bytes(buffer[0:size])

        self.assertEqual(result, b'teaz')

    def test_delete_1(self):
        [self.lib.s2hell_add_character(
            self.ffi.cast('uint8_t', int(i))) for i in b'teaz']

        self.lib.s2hell_move_cursor_backward(10)
        self.lib.s2hell_delete()

        size = self.lib.s2hell_get_line_size()
        buffer = self.lib.s2hell_get_line_buffer()
        result = bytes(buffer[0:size])

        self.assertEqual(result, b'eaz')

    def test_delete_2(self):
        [self.lib.s2hell_add_character(
            self.ffi.cast('uint8_t', int(i))) for i in b'teaz']

        self.lib.s2hell_move_cursor_backward(2)
        self.lib.s2hell_delete()

        size = self.lib.s2hell_get_line_size()
        buffer = self.lib.s2hell_get_line_buffer()
        result = bytes(buffer[0:size])

        self.assertEqual(result, b'tez')

    def test_delete_3(self):
        [self.lib.s2hell_add_character(
            self.ffi.cast('uint8_t', int(i))) for i in b'teaz']

        self.lib.s2hell_delete()

        size = self.lib.s2hell_get_line_size()
        buffer = self.lib.s2hell_get_line_buffer()
        result = bytes(buffer[0:size])

        self.assertEqual(result, b'teaz')

    


