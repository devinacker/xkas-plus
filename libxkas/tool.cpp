int xkas::eval_integer(const char *&s) {
  if(!*s) throw "nall::bad_eval_integer";
  int value = 0, x = *s, y = *(s + 1);

  //label
  if(x == '.' || x == '_' || (x >= 'A' && x <= 'Z') || (x >= 'a' && x <= 'z')) {
    const char *start = s;
    s += 1;

    for(;;) {
      if(*s == ':' || *s == '.' || *s == '_' || (*s >= 'A' && *s <= 'Z') || (*s >= 'a' && *s <= 'z') || (*s >= '0' && *s <= '9')) {
        s++;
        continue;
      }
      break;
    }

    string name;
    strlcpy(name, start, s - start + 1);
    if(decode_label(name) == false) throw "nall::bad_eval_integer";
    unsigned offset;
    if(find_label(name, offset) == true) return offset;
    if(pass == 1) return 0; //label may not have been found in first pass (may occur later on)
    throw "nall::bad_eval_integer";
  }

  //hexadecimal
  if(x == '$' || (x == '0' && (y == 'X' || y == 'x'))) {
    s += (x == '$' ? 1 : 2);
    for(;;) {
      if(*s >= '0' && *s <= '9') { value = value * 16 + (*s++ - '0');      continue; }
      if(*s >= 'A' && *s <= 'F') { value = value * 16 + (*s++ - 'A' + 10); continue; }
      if(*s >= 'a' && *s <= 'f') { value = value * 16 + (*s++ - 'a' + 10); continue; }
      return value;
    }
  }

  //binary
  if(x == '%' || (x == '0' && (y == 'B' || y == 'b'))) {
    s += (x == '%' ? 1 : 2);
    for(;;) {
      if(*s == '0' || *s == '1') { value = value * 2 + (*s++ - '0'); continue; }
      return value;
    }
  }

  //octal (or decimal '0')
  if(x == '0') {
    s += 1;
    for(;;) {
      if(*s >= '0' && *s <= '7') { value = value * 8 + (*s++ - '0'); continue; }
      return value;
    }
  }

  //decimal
  if(x >= '0' && x <= '9') {
    for(;;) {
      if(*s >= '0' && *s <= '9') { value = value * 10 + (*s++ - '0'); continue; }
      return value;
    }
  }

  //char
  if(x == '\'') {
    s += 1;
    value = state.table[*s++];
    if(*s != '\'') throw "mismatched_char";
    s += 1;
    return value;
  }

  throw "nall::bad_eval_integer";
}

int xkas::eval(const char *&s, int depth) {
  if(!*s) throw "nall::bad_eval";
  while(*s == ' ' || *s == '\t') s++; //trim whitespace
  int value = 0, x = *s, y = *(s + 1);

  if(*s == '(') {
    value = eval(++s, 1);
    if(*s++ != ')') throw "nall::bad_eval";
  }

  else if(x == '!') value = !eval(++s, 14);
  else if(x == '~') value = ~eval(++s, 14);
  else if(x == '+') value = +eval(++s, 14);
  else if(x == '-') value = -eval(++s, 14);

  else if(x == '$' || x == '%' || x == '\'' || x == '.' || x == '_'
      || (x >= 'A' && x <= 'Z') || (x >= 'a' && x <= 'z') || (x >= '0' && x <= '9')
  ) value = eval_integer(s);

  else throw "nall::bad_eval";

  for(;;) {
    while(*s == ' ' || *s == '\t') s++; //trim whitespace
    if(!*s) break;
    x = *s, y = *(s + 1);

    if(depth >= 13) break;
    if(x == '*') { value *= eval(++s, 13); continue; }
    if(x == '/') { value /= eval(++s, 13); continue; }
    if(x == '%') { value %= eval(++s, 13); continue; }

    if(depth >= 12) break;
    if(x == '+') { value += eval(++s, 12); continue; }
    if(x == '-') { value -= eval(++s, 12); continue; }

    if(depth >= 11) break;
    if(x == '<' && y == '<') { value <<= eval(++++s, 11); continue; }
    if(x == '>' && y == '>') { value >>= eval(++++s, 11); continue; }

    if(depth >= 10) break;
    if(x == '<' && y == '=') { value = value <= eval(++++s, 10); continue; }
    if(x == '>' && y == '=') { value = value >= eval(++++s, 10); continue; }
    if(x == '<') { value = value < eval(++s, 10); continue; }
    if(x == '>') { value = value > eval(++s, 10); continue; }

    if(depth >= 9) break;
    if(x == '=' && y == '=') { value = value == eval(++++s, 9); continue; }
    if(x == '!' && y == '=') { value = value != eval(++++s, 9); continue; }

    if(depth >= 8) break;
    if(x == '&' && y != '&') { value = value & eval(++s, 8); continue; }

    if(depth >= 7) break;
    if(x == '^' && y != '^') { value = value ^ eval(++s, 7); continue; }

    if(depth >= 6) break;
    if(x == '|' && y != '|') { value = value | eval(++s, 6); continue; }

    if(depth >= 5) break;
    if(x == '&' && y == '&') { value = eval(++++s, 5) && value; continue; }

    if(depth >= 4) break;
    if(x == '^' && y == '^') { value = (!eval(++++s, 4) != !value); continue; }

    if(depth >= 3) break;
    if(x == '|' && y == '|') { value = eval(++++s, 3) || value; continue; }

    if(x == '?') {
      int lhs = eval(++s, 2);
      if(*s != ':') throw "nall::bad_eval";
      int rhs = eval(++s, 2);
      value = value ? lhs : rhs;
      continue;
    }
    if(depth >= 2) break;

    if(depth > 0 && x == ')') break;

    throw "nall::bad_eval";
  }

  return value;
}

int xkas::decode(const char *s) {
  if(!*s) return 0;

  //+ label (special case)
  if(*s == '+' && !*(s + 1)) {
    string name = string() << "+" << (int)state.plus_label_counter;
    unsigned offset;
    if(find_label(name, offset) == true) return offset;
    printf("xkas warning: eval(\"%s\") failed\n", s);
    return 0;
  }

  //- label (special case)
  if(*s == '-' && !*(s + 1)) {
    string name = string() << "-" << (int)state.minus_label_counter - 1;
    unsigned offset;
    if(find_label(name, offset) == true) return offset;
    printf("xkas warning: eval(\"%s\") failed\n", s);
    return 0;
  }

  const char *source = s;
  try {
    return eval(s);
  } catch(const char*) {
    printf("xkas warning: eval(\"%s\") failed\n", source);
    return 0;
  }
}

bool xkas::decode_label(string &s) {
  size_t length = strlen(s);

  //sublabel
  if(s[0] == '.') {
    for(size_t i = 1; i < length - 1; i++) {
      if(s[i] == ':') continue;
      if(s[i] == '_') continue;
      if(s[i] >= '0' && s[i] <= '9') continue;
      if(s[i] >= 'A' && s[i] <= 'Z') continue;
      if(s[i] >= 'a' && s[i] <= 'z') continue;
      return false;
    }
    string temp = string() << state.active_label.name << s;
    s = temp;
    return true;
  }

  //label
  if(s[0] == '_' || (s[0] >= 'A' && s[0] <= 'Z') || (s[0] >= 'a' && s[0] <= 'z')) {
    for(size_t i = 1; i < length - 1; i++) {
      if(s[i] == ':') continue;
      if(s[i] == '_') continue;
      if(s[i] == '.') continue;
      if(s[i] >= '0' && s[i] <= '9') continue; //label cannot start with 0-9
      if(s[i] >= 'A' && s[i] <= 'Z') continue;
      if(s[i] >= 'a' && s[i] <= 'z') continue;
      return false;
    }
    return true;
  }

  return false;
}

bool xkas::find_label(string &name, unsigned &offset) {
  string s = name;

  //attach active namespace prefix, if namespace not specified
  if(!strpos(name, "::")) {
    //verify label is not a +/- label
    if(!strpos(name, "+") && !strpos(name, "-")) {
      s = string() << state.active_namespace << "::" << name;
    }
  }

  for(unsigned i = 0; i < state.label.size(); i++) {
    if(s == state.label[i].name) {
      offset = state.label[i].offset;
      return true;
    }
  }

  //labels may not have been reached yet on the first pass
  if(pass == 1) {
    offset = 0;
    return true;
  }

  return false;
}
