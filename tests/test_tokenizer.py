import cffi
import unittest

class TestS2HellTokenizer(unittest.TestCase):
    def setUp(self):
        self.ffi = cffi.FFI()
        with open("output/s2hell.h.preprocessed") as f:
            self.ffi.cdef(f.read())

        self.lib = self.ffi.dlopen("output/s2hell.so")

    def test_jump_space(self):
        cmd = bytes("   test", "utf8")
        position = self.ffi.new("uint8_t *", 0)
        self.lib.jump_space(cmd, position)

        self.assertEqual(position[0], 3)

    def test_token_word(self):
        cmd = bytes("test word", "utf8")
        position = 0 # test
        length = self.ffi.new("uint8_t *", 0)
        self.lib.token_word(cmd, position, length)

        self.assertEqual(length[0], 4)

    def test_token_word_eos(self):
        cmd = bytes("test words", "utf8")
        position = 5 # words
        length = self.ffi.new("uint8_t *", 0)
        self.lib.token_word(cmd, position, length)

        self.assertEqual(length[0], 5)

    def test_token_quote_1_2(self):
        cmd = bytes('"test of word"   ', "utf8")
        position = self.ffi.new("uint8_t *", 0) # "test"
        length = self.ffi.new("uint8_t *", 0)
        double_quote = self.ffi.cast('char', 34)
        self.lib.token_quotes(cmd, double_quote, position, length)

        self.assertEqual(position[0], 1)

    def test_token_quote_2_2(self):
        cmd = bytes('"test of word"   ', "utf8")
        position = self.ffi.new("uint8_t *", 0) # "test"
        length = self.ffi.new("uint8_t *", 0)
        double_quote = self.ffi.cast('char', 34)
        self.lib.token_quotes(cmd, double_quote, position, length)

        self.assertEqual(length[0], 12)

    def test_token_quote_inner(self):
        cmd = bytes('"AAAA \\"bb\\" cccc"   ', "utf8")
        position = self.ffi.new("uint8_t *", 0) # "test"
        length = self.ffi.new("uint8_t *", 0)
        double_quote = self.ffi.cast('char', 34)
        self.lib.token_quotes(cmd, double_quote, position, length)

        self.assertEqual(length[0], 16)

    def test_s2hell_tokenize(self):
        cmd = bytes('xxx aaa bbb', "utf8")
        tokens = self.ffi.new("s2hell_token[10]")
        result = self.lib.s2hell_tokenize(cmd, tokens, 10)
        # self.lib.s2hell_show_tokens(cmd, tokens, 10)

        # expect to find 1 command, 2 words and the eol
        self.assertEqual(result, 4)

    def test_s2hell_tokenize_2_double_quoted(self):
        cmd = bytes('xxx aaa "ccc is here" bbb', "utf8")
        tokens = self.ffi.new("s2hell_token[10]")
        result = self.lib.s2hell_tokenize(cmd, tokens, 10)
        #self.lib.s2hell_show_tokens(cmd, tokens, 10)

        # expect to find 1 command, 2 words (one quoted) and the eol
        self.assertEqual(result, 5)

    def test_s2hell_tokenize_2_simple_quoted(self):
        cmd = bytes("xxx aaa 'ccc is here' bbb", "utf8")
        tokens = self.ffi.new("s2hell_token[10]")
        result = self.lib.s2hell_tokenize(cmd, tokens, 10)
        #self.lib.s2hell_show_tokens(cmd, tokens, 10)

        # expect to find 1 command, 2 words (one quoted) and the eol
        self.assertEqual(result, 5)

    def test_s2hell_tokenize_2_unfinished(self):
        cmd = bytes('xxx aaa "ccc is here bbb', "utf8")
        tokens = self.ffi.new("s2hell_token[10]")
        result = self.lib.s2hell_tokenize(cmd, tokens, 10)
        #self.lib.s2hell_show_tokens(cmd, tokens, 10)

        # expect to fail since the quoting is note finished !
        self.assertEqual(result, -1)

    def test_s2hell_tokenize_3(self):
        cmd = bytes('xxx aaa bbb\n dd ff gg', "utf8")
        tokens = self.ffi.new("s2hell_token[10]")
        result = self.lib.s2hell_tokenize(cmd, tokens, 10)
        #self.lib.s2hell_show_tokens(cmd, tokens, 10)

        # expect to ignore what append after the \n
        self.assertEqual(result, 4)

    def test_s2hell_tokenize_4(self):
        cmd = bytes('xxx aaa "bbb"\n dd ff gg', "utf8")
        tokens = self.ffi.new("s2hell_token[10]")
        result = self.lib.s2hell_tokenize(cmd, tokens, 10)
        #self.lib.s2hell_show_tokens(cmd, tokens, 10)

        # expect to ignore what append after the \n
        self.assertEqual(result, 4)

    def test_s2hell_tokenize_bad_char(self):
        # must fail since % is not a valid char
        cmd = bytes('xxx aaa % yyy ff', "utf8")
        tokens = self.ffi.new("s2hell_token[10]")
        result = self.lib.s2hell_tokenize(cmd, tokens, 10)
        #self.lib.s2hell_show_tokens(cmd, tokens, 10)

        # expect to ignore what append after the \n
        self.assertNotEqual(result, 0)

    def test_s2hell_tokenize_pipe(self):
        cmd = bytes('xxx aaa | yyy ff', "utf8")
        tokens = self.ffi.new("s2hell_token[10]")
        result = self.lib.s2hell_tokenize(cmd, tokens, 10)
        #self.lib.s2hell_show_tokens(cmd, tokens, 10)

        # expect to ignore what append after the \n
        self.assertEqual(result, 6)

    def test_s2hell_tokenize_double_pipe(self):
        # not supported yet
        cmd = bytes('xxx aaa || yyy ff', "utf8")
        tokens = self.ffi.new("s2hell_token[10]")
        result = self.lib.s2hell_tokenize(cmd, tokens, 10)
        #self.lib.s2hell_show_tokens(cmd, tokens, 10)

        # expect to ignore what append after the \n
        self.assertEqual(result, -1)

    def test_s2hell_tokenize_ending_pipe(self):
        cmd = bytes('xxx aaa |', "utf8")
        tokens = self.ffi.new("s2hell_token[10]")
        result = self.lib.s2hell_tokenize(cmd, tokens, 10)
        #self.lib.s2hell_show_tokens(cmd, tokens, 10)

        # expect to ignore what append after the \n
        self.assertEqual(result, -1)

    def test_s2hell_tokenize_begining_pipe(self):
        cmd = bytes('| xxx aaa', "utf8")
        tokens = self.ffi.new("s2hell_token[10]")
        result = self.lib.s2hell_tokenize(cmd, tokens, 10)
        #self.lib.s2hell_show_tokens(cmd, tokens, 10)

        # expect to ignore what append after the \n
        self.assertEqual(result, -1)

    
