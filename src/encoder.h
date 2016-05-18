#ifndef ENCODER_INCLUDED
#define ENCODER_INCLUDED

#include "trans.h"

template<class Writer>
struct Encoder {
  const Machine& machine;
  Writer& outs;
  State current;
  bool msb0;  // set this to encode MSB first, instead of LSB first
  Encoder (const Machine& machine, Writer& outs)
    : machine(machine),
      outs(outs),
      current(machine.startState()),
      msb0(false)
  { }

  void write (char outc) {
    if (outc)
      (void) outs.write (&outc, 1);
  }
  
  void encodeSymbol (char sym) {
    LogThisAt(8,"Encoding " << sym << endl);
    while (!machine.state[current].hasInput()) {
      const MachineTransition& tn = machine.state[current].next();
      write (tn.out);
      LogThisAt(9,"Transition " << machine.state[current].name
		<< " -> " << machine.state[tn.dest].name
		<< ": output " << tn.out
		<< endl);
      current = tn.dest;
    }
    const MachineTransition* t = machine.state[current].transFor (sym);
    Assert (t != NULL, "Couldn't encode symbol %c in state %s", sym, machine.state[current].name.c_str());
    while (true) {
      write (t->out);
      LogThisAt(9,"Transition " << machine.state[current].name
		<< " -> " << machine.state[t->dest].name << ": "
		<< (t->in ? (string("input ") + t->in + ", ") : string())
		<< "output " << t->out
		<< endl);
      current = t->dest;
      if (machine.state[current].hasInput())
	break;
      t = &machine.state[current].next();
    }
  }

  void encodeBit (bool bit) {
    encodeSymbol (bit ? '1' : '0');
  }

  void encodeMeta (ControlIndex control) {
    encodeSymbol (Machine::controlChar (control));
  }

  template<typename CharType>
  void encodeByte (CharType byte) {
    LogThisAt(7,"Encoding '" << (char)byte << "' (\\x" << hex << (int)byte << ")" << endl);
    if (msb0)
      for (int n = 7; n >= 0; --n)
	encodeBit (byte & (1 << n));
    else
      for (int n = 0; n <= 7; ++n)
	encodeBit (byte & (1 << n));
  }

  void encodeStream (istream& in) {
    istreambuf_iterator<char> iter(in), iterEnd;
    while (iter != iterEnd) {
      encodeByte (*iter);
      ++iter;
    }
  }

  void encodeString (const string& s) {
    for (auto c: s)
      encodeByte (c);
  }

  void encodeSymbolString (const string& s) {
    for (auto c: s)
      encodeSymbol (c);
  }
};

struct FastaWriter {
  ostream& outs;
  size_t col, colsPerLine;
  
  FastaWriter (ostream& outs, const char* seqname = "SEQ")
    : outs(outs),
      col(0),
      colsPerLine(seqname == NULL ? 0 : 50)
  {
    if (seqname != NULL)
      outs << ">" << seqname << endl;
  }

  ~FastaWriter() {
    if (col > 0)
      outs << endl;
  }
  
  void write (char* buf, size_t n) {
    for (size_t i = 0; i < n; ++i) {
      outs << buf[i];
      if (++col >= colsPerLine && colsPerLine > 0) {
	outs << endl;
	col = 0;
      }
    }
  }
};

#endif /* ENCODER_INCLUDED */
