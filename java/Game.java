package java;

import java.io.IOException;
import java.io.InputStream;
import java.io.StreamTokenizer;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Random;

class Prefix {
    private List<String> pref;

    // Copy constructor
    Prefix(Prefix p) {
        pref = new ArrayList<>(p.pref);
    }

    // Constructor with n identical strings
    Prefix(int n, String str) {
        pref = new ArrayList<>();
        for (int i = 0; i < n; i++) {
            pref.add(str);
        }
    }

    static final int MULTIPLIER = 31; // for hashCode()

    // Generate hash from all prefix words
    @Override
    public int hashCode() {
        int h = 0;
        for (String word : pref) {
            h = MULTIPLIER * h + word.hashCode();
        }
        return h;
    }

    // Compare two prefixes for equal words
    @Override
    public boolean equals(Object o) {
        if (this == o)
            return true;
        if (o == null || getClass() != o.getClass())
            return false;

        Prefix prefix = (Prefix) o;
        return pref.equals(prefix.pref);
    }

    // Get the list of prefix words
    public List<String> getPref() {
        return pref;
    }
}

class Chain {
    static final int NPREF = 2;
    static final String NONWORD = "\n";

    private Map<Prefix, List<String>> statetab = new HashMap<>();
    private Prefix prefix = new Prefix(NPREF, NONWORD);
    private Random rand = new Random();

    // Build state table from input stream
    void build(InputStream in) throws IOException {
        StreamTokenizer st = new StreamTokenizer(new java.io.InputStreamReader(in));

        st.resetSyntax();
        st.wordChars(0, Character.MAX_VALUE); // Turn on all chars
        st.whitespaceChars(0, ' ');

        while (st.nextToken() != StreamTokenizer.TT_EOF) {
            add(st.sval);
        }
        add(NONWORD);
    }

    // Add word to suffix list, update prefix
    void add(String word) {
        List<String> suf = statetab.get(prefix);
        if (suf == null) {
            suf = new ArrayList<>();
            statetab.put(new Prefix(prefix), suf);
        }

        suf.add(word);

        // Update prefix: remove first word, add new word
        List<String> prefList = prefix.getPref();
        prefList.remove(0);
        prefList.add(word);
    }

    // Generate output words
    void generate(int nwords) {
        prefix = new Prefix(NPREF, NONWORD);

        for (int i = 0; i < nwords; i++) {
            List<String> suffixes = statetab.get(prefix);

            if (suffixes == null || suffixes.isEmpty()) {
                break; // No suffixes available
            }

            // Use proper random selection without modulo bias
            int r = rand.nextInt(suffixes.size());
            String suf = suffixes.get(r);

            if (suf.equals(NONWORD)) {
                break;
            }

            System.out.println(suf);

            // Update prefix: remove first word, add selected word
            List<String> prefList = prefix.getPref();
            prefList.remove(0);
            prefList.add(suf);
        }
    }
}

class Markov {
    static final int MAXGEN = 10000;

    public static void main(String[] args) {
        Chain chain = new Chain();
        int nwords = MAXGEN;

        try {
            chain.build(System.in);
            chain.generate(nwords);
        } catch (IOException e) {
            System.err.println("Error reading input: " + e.getMessage());
            e.printStackTrace();
        }
    }
}
