
#include "id.h"
#include "expression.h"
#include "statement.h"
#include "module.h"
#include "coercer.h"

using namespace hilti;

void Coercer::visit(type::Integer* i)
{
    setResult(false);

    auto dst_b = ast::as<type::Bool>(arg1());

    if ( dst_b ) {
        setResult(true);
        return;
    }

    // We don't allow integer coercion to larger widths here because we
    // wouldn't know if they are signed or not.
}

void Coercer::visit(type::Reference* r)
{
    setResult(false);

    auto dst_ref = ast::as<type::Reference>(arg1());

    if ( dst_ref ) {

        if ( ast::isA<type::RegExp>(r->argType()) && ast::isA<type::RegExp>(dst_ref->argType()) ) {
            setResult(true);
            return;
        }

        setResult(dst_ref->wildcard());
        return;
    }

    auto dst_b = ast::as<type::Bool>(arg1());

    if ( dst_b ) {
        setResult(true);
        return;
    }
}

void Coercer::visit(type::Iterator* i)
{
    setResult(false);

    auto dst_iter = ast::as<type::Iterator>(arg1());

    if ( dst_iter ) {
        setResult(dst_iter->wildcard());
        return;
    }
}

void Coercer::visit(type::Tuple* t)
{
    setResult(false);

    auto rtype = ast::as<type::Reference>(arg1());

    if ( rtype ) {
        auto stype = ast::as<type::Struct>(rtype->argType());

        if ( ! stype )
            return;

        // Coerce to struct is fine if (1) it's an emptuple, or (2) all
        // element coerce.

        if ( t->typeList().size() == 0 ) {
	    setResult(true);
	    return;
	}

        if ( t->typeList().size() != stype->typeList().size() )
            return;

        for ( auto i : util::zip2(t->typeList(), stype->typeList()) ) {

            if ( ast::as<type::Unset>(i.first) )
                continue;

            if ( ! canCoerceTo(i.first, i.second) )
                return;
        }

        setResult(true);
        return;
    }

    auto dst = ast::as<type::Tuple>(arg1());

    if ( ! dst )
        return;

    if ( dst->wildcard() ) {
        setResult(true);
        return;
    }

    auto dst_types = dst->typeList();

    if ( dst_types.size() != t->typeList().size() )
        return;

    auto d = dst_types.begin();

    for ( auto e : t->typeList() ) {
        if ( ! canCoerceTo(e, *d++) )
            return;
    }

    setResult(true);
}

void Coercer::visit(type::Address* t)
{
    setResult(false);

    auto dst_net = ast::as<type::Network>(arg1());

    if ( dst_net ) {
        setResult(true);
        return;
    }
}

void Coercer::visit(type::CAddr* c)
{
    setResult(false);

    auto dst_b = ast::as<type::Bool>(arg1());

    if ( dst_b ) {
        setResult(true);
        return;
    }
}

void Coercer::visit(type::Unset* c)
{
    // Generic unsets coerce into any type as the corresponding default
    // value.
    setResult(true);
}

void Coercer::visit(type::Union* c)
{
    setResult(false);

    auto dst_b = ast::as<type::Bool>(arg1());

    if ( dst_b ) {
        setResult(true);
        return;
    }
}
