import cffi
import unittest

class TestS2HellStd(unittest.TestCase):
    def setUp(self):
        self.ffi = cffi.FFI()
        with open("output/s2hell.h.preprocessed") as f:
            self.ffi.cdef(f.read())

        self.lib = self.ffi.dlopen("output/x86/s2hell.so")
        # mandatory to prevent side effect !
        self.lib.s2hell_reset_stds()

    def test_s2hell_get_current_stdout(self):
        result = self.lib.s2hell_get_current_stdout()
        self.assertEqual(result, 0)

    def test_s2hell_get_current_stdin(self):
        result = self.lib.s2hell_get_current_stdin()
        self.assertEqual(result, 1)

    def test_s2hell_get_current_stdout_after_swap(self):
        self.lib.s2hell_swap_stds()
        result = self.lib.s2hell_get_current_stdout()
        self.assertEqual(result, 1)

    def test_s2hell_get_current_stdin_after_swap(self):
        self.lib.s2hell_swap_stds()
        result = self.lib.s2hell_get_current_stdin()
        self.assertEqual(result, 0)

    def test_process_raw_write_on_stdout(self):
        # This test is complicated : the write needs two function calls + 1 to check

        available = self.ffi.new("uint8_t *", 0)
        buffer = self.lib.s2hell_get_current_stdout_buffer(available)
        buffer[0] = self.ffi.cast('char', 66)
        buffer[1] = self.ffi.cast('char', 67)
        buffer[2] = self.ffi.cast('char', 68)
        self.lib.s2hell_stdout_report_write(3)

        stdout = self.lib.s2hell_get_complet_stdout_buffer()

        self.assertEqual(self.ffi.string(stdout), b'BCD')

    def test_process_raw_write_on_stdout_2(self):
        # This test is complicated : the write needs two function calls + 1 to check

        available = self.ffi.new("uint8_t *", 0)
        buffer = self.lib.s2hell_get_current_stdout_buffer(available)
        buffer[0] = self.ffi.cast('char', 66)
        buffer[1] = self.ffi.cast('char', 67)
        buffer[2] = self.ffi.cast('char', 68)
        self.lib.s2hell_stdout_report_write(3)
        buffer = self.lib.s2hell_get_current_stdout_buffer(available)
        buffer[0] = self.ffi.cast('char', 32)
        buffer[1] = self.ffi.cast('char', 69)
        buffer[2] = self.ffi.cast('char', 70)
        self.lib.s2hell_stdout_report_write(3)

        stdout = self.lib.s2hell_get_complet_stdout_buffer()

        self.assertEqual(self.ffi.string(stdout), b'BCD EF')

    def test_process_raw_write_on_stdout_3(self):
        # This test is complicated : the write needs two function calls + 1 to check

        available = self.ffi.new("uint8_t *", 0)
        buffer = self.lib.s2hell_get_current_stdout_buffer(available)
        available = int(self.ffi.cast("int", available[0]))
        for i in range(available):
            buffer[i] = self.ffi.cast('char', 66)

        self.lib.s2hell_stdout_report_write(available)

        stdout = self.lib.s2hell_get_complet_stdout_buffer()    
        stdout_string = self.ffi.string(stdout)

        self.assertEqual(len(stdout_string), 127)

    def test_process_read_on_stdin(self):
        # This test is complicated : the write needs two function calls + 1 to check

        available = self.ffi.new("uint8_t *", 0)
        buffer = self.lib.s2hell_get_current_stdout_buffer(available)
        buffer[0] = self.ffi.cast('char', 66)
        buffer[1] = self.ffi.cast('char', 67)
        buffer[2] = self.ffi.cast('char', 68)
        self.lib.s2hell_stdout_report_write(3)
        self.lib.s2hell_swap_stds() # next command

        stdin = self.lib.s2hell_get_complet_stdin_buffer()

        self.assertEqual(self.ffi.string(stdin), b'BCD')

    def test_s2hell_stdout_write(self):
        self.lib.s2hell_stdout_write(b'ABC')

        stdout = self.lib.s2hell_get_complet_stdout_buffer()

        self.assertEqual(self.ffi.string(stdout), b'ABC')

    def test_s2hell_stdout_write_2(self):
        self.lib.s2hell_stdout_write(b'ABC')
        self.lib.s2hell_stdout_write(b'DEF')

        stdout = self.lib.s2hell_get_complet_stdout_buffer()

        self.assertEqual(self.ffi.string(stdout), b'ABCDEF')

    def test_s2hell_stdout_nwrite(self):
        self.lib.s2hell_stdout_nwrite(b'ABCD', 3)

        stdout = self.lib.s2hell_get_complet_stdout_buffer()

        self.assertEqual(self.ffi.string(stdout), b'ABC')

    def test_s2hell_stdout_nwrite_2(self):
        self.lib.s2hell_stdout_nwrite(b'ABCD', 3)
        self.lib.s2hell_stdout_nwrite(b'DEFG', 3)

        stdout = self.lib.s2hell_get_complet_stdout_buffer()

        self.assertEqual(self.ffi.string(stdout), b'ABCDEF')

    def test_s2hell_stdin_size(self):
        self.lib.s2hell_stdout_write(b'ABC')

        self.lib.s2hell_swap_stds() # next command
        result = self.lib.s2hell_get_stdin_size()
        stdin_result = int(self.ffi.cast("int", result))

        self.assertEqual(stdin_result, 3)

    def test_s2hell_clear_stdout(self):
        self.lib.s2hell_stdout_write(b'ABC')
        self.lib.s2hell_clear_stdout()
        stdout = self.lib.s2hell_get_complet_stdout_buffer()

        self.assertEqual(self.ffi.string(stdout), b'')
