import cffi
import unittest

class TestS2HellCommands(unittest.TestCase):
    def setUp(self):
        self.ffi = cffi.FFI()
        with open("output/s2hell.h.preprocessed") as f:
            self.ffi.cdef(f.read())

        self.lib = self.ffi.dlopen("output/x86/s2hell.so")
        # mandatory to prevent side effect !
        self.lib.s2hell_reset_stds()
        self.lib.s2hell_set_default_x86_commands()

    def test_echo_return(self):
        cmd = bytes('echo "this is a test"', "utf8")
        tokens = self.ffi.new("s2hell_token[10]")
        self.lib.s2hell_tokenize(cmd, tokens, 10)
        result = self.lib.s2hell_execute(cmd, tokens, 10)
        result = int(self.ffi.cast("int", result))

        self.assertEqual(result, 0)

    def test_echo_stdout(self):
        cmd = bytes('echo "this is a test\nand another one"', "utf8")
        tokens = self.ffi.new("s2hell_token[10]")
        self.lib.s2hell_tokenize(cmd, tokens, 10)
        self.lib.s2hell_execute(cmd, tokens, 10)
        stdout = self.lib.s2hell_get_complet_stdout_buffer()

        self.assertEqual(self.ffi.string(stdout), b'this is a test\nand another one\n')