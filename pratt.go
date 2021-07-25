// pratt parser implementation
// blog post: https://bingxio.fun/
package main

import (
	"fmt"
)

const (
	Number = iota
	Ident
	Add
	Sub
	Mul
	Div
	Lp
	Rp
	Gr
	GrEq
	Le
	LeEq
	EqEq
	NotEq
	Or
	And
	Dot
	Com
	Eof
)

const (
	lowest  = iota
	or      // |
	and     // &
	eq      // == !=
	compare // > >= < <=
	term    // + -
	factor  // * /
	unary   // - !
	call    // . () []
)

type token struct {
	kind    int
	literal string
}

type rule struct {
	prefix     func()
	infix      func()
	precedence int
}

type parser struct {
	iter func() token
	pre  token
	cur  token
	rule map[int]rule
}

func (p *parser) next() {
	p.pre = p.cur
	p.cur = p.iter()
}

func (p *parser) register() {
	p.rule[Number] = rule{p.literal, nil, lowest}
	p.rule[Ident] = rule{p.literal, nil, lowest}

	p.rule[Add] = rule{nil, p.binary, term}
	p.rule[Sub] = rule{p.unary, p.binary, term}

	p.rule[Mul] = rule{nil, p.binary, factor}
	p.rule[Div] = rule{nil, p.binary, factor}

	p.rule[Or] = rule{nil, p.binary, or}
	p.rule[And] = rule{nil, p.binary, and}

	p.rule[EqEq] = rule{nil, p.binary, eq}
	p.rule[NotEq] = rule{nil, p.binary, eq}

	p.rule[Gr] = rule{nil, p.binary, compare}
	p.rule[GrEq] = rule{nil, p.binary, compare}
	p.rule[Le] = rule{nil, p.binary, compare}
	p.rule[LeEq] = rule{nil, p.binary, compare}

	p.rule[Dot] = rule{nil, p.get, call}
	p.rule[Lp] = rule{p.group, p.call, call}
}

func (p parser) getPrePrecedence() int {
	if v, ok := p.rule[p.pre.kind]; ok {
		return v.precedence
	}
	return lowest
}

func (p parser) getCurPrecedence() int {
	if v, ok := p.rule[p.cur.kind]; ok {
		return v.precedence
	}
	return lowest
}

func (p *parser) goPrecedence(precedence int) {
	r := p.rule[p.pre.kind]
	if r.prefix == nil {
		panic("no parse prefix function: " + p.pre.literal)
	}
	r.prefix()
	for precedence < p.getCurPrecedence() {
		r := p.rule[p.cur.kind]
		p.next()
		r.infix()
	}
}

func (p *parser) literal() {
	fmt.Println(p.pre.literal)
}

func (p *parser) unary() {
	op := p.pre
	p.next()
	p.goPrecedence(unary)
	fmt.Println("UNARY", op.literal)
}

func (p *parser) binary() {
	op := p.pre
	pr := p.getPrePrecedence()
	p.next()
	p.goPrecedence(pr)
	fmt.Println("BIN", op.literal)
}

func (p *parser) group() {
	p.next()
	p.goPrecedence(lowest)
	if p.cur.kind != Rp {
		panic("lost right paren symbol")
	} else {
		p.next()
	}
}

func (p *parser) get() {
	p.next()
	fmt.Println("GET", p.pre.literal)
}

func (p *parser) call() {
	p.next()
	count := 0
	for p.pre.kind != Rp {
		p.goPrecedence(lowest)
		count++
		p.next()
		if p.pre.kind == Rp {
			break
		} else if p.pre.kind != Com {
			panic("lost comma symbol in arguments")
		}
		p.next()
	}
	fmt.Println("CALL", count)
}

func main() {
	tokens := [][]token{
		{
			// -3 + 4 * (5 + 2) - 8
			{Sub, "-"},
			{Number, "3"},
			{Add, "+"},
			{Number, "4"},
			{Mul, "*"},
			{Lp, "("},
			{Number, "5"},
			{Add, "+"},
			{Number, "2"},
			{Rp, ")"},
			{Sub, "-"},
			{Number, "8"},
			{Eof, "eof"},
		},
		{
			// 2 + 3 * 4
			{Number, "2"},
			{Add, "+"},
			{Number, "3"},
			{Mul, "*"},
			{Number, "4"},
			{Eof, "eof"},
		},
		{
			// -3 * 2
			{Sub, "-"},
			{Number, "3"},
			{Mul, "*"},
			{Number, "2"},
			{Eof, "eof"},
		},
		{
			// a + b < c == 5
			{Ident, "a"},
			{Add, "+"},
			{Ident, "b"},
			{Le, "<"},
			{Ident, "c"},
			{EqEq, "=="},
			{Number, "5"},
			{Eof, "eof"},
		},
		{
			// a | b
			{Ident, "a"},
			{Or, "|"},
			{Ident, "b"},
			{Eof, "eof"},
		},
		{
			// -(a + b) + c - 1 & b + c / 2
			{Sub, "-"},
			{Lp, "("},
			{Ident, "a"},
			{Add, "+"},
			{Ident, "b"},
			{Rp, ")"},
			{Add, "+"},
			{Ident, "c"},
			{Sub, "-"},
			{Number, "1"},
			{And, "&"},
			{Ident, "b"},
			{Add, "+"},
			{Ident, "c"},
			{Div, "/"},
			{Number, "2"},
			{Eof, "eof"},
		},
		{
			// x.a > y & c > x.b
			{Ident, "x"},
			{Dot, "."},
			{Ident, "a"},
			{Gr, ">"},
			{Ident, "y"},
			{And, "&"},
			{Ident, "c"},
			{Gr, ">"},
			{Ident, "x"},
			{Dot, "."},
			{Ident, "b"},
			{Eof, "eof"},
		},
		{
			// x + foo() * y.z
			{Ident, "x"},
			{Add, "+"},
			{Ident, "foo"},
			{Lp, "("},
			{Rp, ")"},
			{Mul, "*"},
			{Ident, "y"},
			{Dot, "."},
			{Ident, "z"},
			{Eof, "eof"},
		},
		{
			// foo(1, a + b, x.y, a + b * c)
			{Ident, "foo"},
			{Lp, "("},
			{Number, "1"},
			{Com, ","},
			{Ident, "a"},
			{Add, "+"},
			{Ident, "b"},
			{Com, ","},
			{Ident, "x"},
			{Dot, "."},
			{Ident, "y"},
			{Com, ","},
			{Ident, "a"},
			{Add, "+"},
			{Ident, "b"},
			{Mul, "*"},
			{Ident, "c"},
			{Rp, ")"},
			{Eof, "eof"},
		},
	}
	for k, v := range tokens {
		fmt.Println("=== RUNNING: ", k+1, "===")
		p := parser{
			iter: func(tokens []token) func() token {
				i := -1
				return func() token {
					i++
					return tokens[i]
				}
			}(v),
			rule: make(map[int]rule),
		}
		p.next()
		p.next()
		p.register()
		p.goPrecedence(lowest)
	}
}
