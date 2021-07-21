// pratt parser implementation
// blog post: https://bingxio.fun/
package main

import "fmt"

const (
	Lowest = iota

	Number
	Plus
	Minus
	Asterisk
	Slash
	LParen
	RParen

	Eof

	Unary
)

var precedence = map[int]int{
	Number:   1,
	Plus:     2,
	Minus:    2,
	Asterisk: 3,
	Slash:    3,
	LParen:   4,
}

type token struct {
	kind    int
	literal string
}

func main() {
	//tokens := []token{ /* 2 + 3 * 4 + (5 + 6) */
	//	{kind: Number, literal: "2"},
	//	{kind: Plus},
	//	{kind: Number, literal: "3"},
	//	{kind: Asterisk},
	//	{kind: Number, literal: "4"},
	//	{kind: Plus},
	//	{kind: LParen},
	//	{kind: Number, literal: "5"},
	//	{kind: Plus},
	//	{kind: Number, literal: "6"},
	//	{kind: RParen},
	//	{kind: Eof},
	//}
	//tokens := []token{ /* -3 * 5 */
	//	{kind: Minus},
	//	{kind: Number, literal: "3"},
	//	{kind: Asterisk},
	//	{kind: Number, literal: "5"},
	//	{kind: Eof},
	//}
	//tokens := []token{ /* (6 + 3) / (5 / 2) */
	//	{kind: LParen},
	//	{kind: Number, literal: "6"},
	//	{kind: Plus},
	//	{kind: Number, literal: "3"},
	//	{kind: RParen},
	//	{kind: Slash},
	//	{kind: LParen},
	//	{kind: Number, literal: "5"},
	//	{kind: Slash},
	//	{kind: Number, literal: "2"},
	//	{kind: RParen},
	//	{kind: Eof},
	//}
	//tokens := []token{ /* 2 + 3 * 4 */
	//	{kind: Number, literal: "2"},
	//	{kind: Plus},
	//	{kind: Number, literal: "3"},
	//	{kind: Asterisk},
	//	{kind: Number, literal: "4"},
	//	{kind: Eof},
	//}
	tokens := []token{ /* 2 + 3 + 4 */
		{kind: Number, literal: "2"},
		{kind: Plus},
		{kind: Number, literal: "3"},
		{kind: Plus},
		{kind: Number, literal: "4"},
		{kind: Eof},
	}
	p := parser{
		iter: func(tokens []token) func() token {
			i := -1
			return func() token {
				i++
				return tokens[i]
			}
		}(tokens),
		prefix: map[int]func(){},
		infix:  map[int]func(){},
	}
	p.parse()
}

type parser struct {
	iter   func() token
	cur    token
	peek   token
	prefix map[int]func()
	infix  map[int]func()
}

func (p *parser) parse() {
	p.prefix[Number] = p.number
	p.prefix[Minus] = p.unary
	p.prefix[LParen] = p.group

	p.infix[Plus] = p.binary
	p.infix[Minus] = p.binary
	p.infix[Asterisk] = p.binary
	p.infix[Slash] = p.binary

	p.nextToken()
	p.nextToken()

	p.parsePrecedence(Lowest)
}

func (p *parser) parsePrecedence(precedence int) {
	prefix := p.prefix[p.cur.kind]
	if prefix == nil {
		panic("no prefix parse function")
	}
	prefix()
	for precedence < p.getPeekPrecedence() {
		infix := p.infix[p.peek.kind]
		if infix == nil {
			panic("no infix parse function")
		}
		p.nextToken()
		infix()
	}
}

func (p *parser) nextToken() {
	p.cur = p.peek
	p.peek = p.iter()
}

func (p parser) getCurPrecedence() int {
	if v, ok := precedence[p.cur.kind]; ok {
		return v
	}
	return Lowest
}

func (p parser) getPeekPrecedence() int {
	if v, ok := precedence[p.peek.kind]; ok {
		return v
	}
	return Lowest
}

func (p *parser) number() {
	fmt.Println("LOAD_CONST", p.cur.literal)
}

func (p *parser) unary() {
	p.nextToken()
	p.parsePrecedence(Unary)
	fmt.Println("NOT")
}

func (p *parser) binary() {
	op := p.cur

	p.nextToken()
	p.parsePrecedence(p.getCurPrecedence() + 1)

	switch op.kind {
	case Plus:
		fmt.Println("PLUS")
	case Minus:
		fmt.Println("MINUS")
	case Asterisk:
		fmt.Println("ASTERISK")
	case Slash:
		fmt.Println("SLASH")
	}
}

func (p *parser) group() {
	p.nextToken()
	p.parsePrecedence(Lowest)

	p.nextToken()
	if p.cur.kind != RParen {
		panic("lost right paren symbol")
	}
}
