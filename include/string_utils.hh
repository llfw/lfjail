/*
 * This source code is released into the public domain.
 */

#ifndef	LFJAIL_STRING_UTILS_HH
#define	LFJAIL_STRING_UTILS_HH

#include <locale>
#include <ranges>

namespace lfjail {

/*
 * ctype_is: wrap std::ctype<T>::is() in a form suitable for use as an algorithm
 * predicate, i.e., ctype_is(m) will return a functor object that takes any char
 * type as an argument and returns bool.
 *
 * If the locale is not specified, the C locale is used by default.
 */
struct ctype_is_fn {
private:
	/*
	 * This functor is what ctype_is() returns to the caller, and
	 * implements the actual operation based on the provided mask
	 * and locale.  Do it this way so that the functor can be a
	 * template function itself, rather than requiring the caller
	 * to provide a template argument for ctype_is.
	 */
	struct impl {
		impl(std::ctype_base::mask mask, const std::locale &locale)
		    : _mask(mask)
		    , _locale(locale)
		{}

		template<std::integral CharT>
		bool operator()(CharT c) const {
			return (_get_ctype<CharT>().is(_mask, c));
		}

	private:
		std::ctype_base::mask _mask;
		const std::locale &_locale;

		template<std::integral CharT>
		const std::ctype<CharT> &_get_ctype() const {
			return (std::use_facet<std::ctype<CharT>>(_locale));
		}
	};

public:
	/*
	 * Create an instance of the functor and return it.
	 */
	impl operator()(std::ctype_base::mask mask,
	    const std::locale &locale = std::locale::classic()) const
	{
		return (impl(mask, locale));
	}

};
inline constexpr auto ctype_is = ctype_is_fn{};

/*
 * Remove leading whitespace from the given string_view.
 */

template<typename Char>
std::basic_string_view<Char>
skipws(std::basic_string_view<Char> text) {
	auto isspace = ctype_is(std::ctype_base::space);
	auto first_nonws = std::ranges::find_if_not(text, isspace);
	return {first_nonws, end(text)};
}

template<typename Char>
void skipws(std::basic_string_view<Char> *text) {
	auto ret = skipws(*text);
	*text = ret;
}

/*
 * Split a string on a predicate.  Specifically, divide the string into two
 * strings at the first character where the predicate matches, and return
 * both strings.  If the predicate matched, the matching character will be
 * discarded.
 */

template<typename Char,
	 std::indirect_unary_predicate<
	 	typename std::basic_string_view<Char>::iterator> Pred>
std::pair<std::basic_string_view<Char>, std::basic_string_view<Char>>
split(std::basic_string_view<Char> text, Pred pred)
{
	auto split_point = std::ranges::find_if(text, pred);

	if (split_point == end(text))
		return {text, {}};
	else
		return {{begin(text), split_point},
			{std::next(split_point), end(text)}};
}

template<typename Char,
	 std::indirect_unary_predicate<
	 	typename std::basic_string_view<Char>::iterator> Pred>
std::basic_string_view<Char>
split(std::basic_string_view<Char> *text, Pred pred)
{
	auto [str, rest] = split(*text, pred);
	*text = rest;
	return str;
}

/*
 * Return the next word from a string_view.  The by-value version returns
 * a pair<next-word, remainder>, while the pointer version returns next-word
 * and sets the input string to remainder.
 */

template<typename Char>
std::pair<std::basic_string_view<Char>, std::basic_string_view<Char>>
next_word(std::basic_string_view<Char> text)
{
	return (split(skipws(text), ctype_is(std::ctype_base::space)));
}

template<typename Char>
std::basic_string_view<Char>
next_word(std::basic_string_view<Char> *text) {
	skipws(text);
	return (split(text, ctype_is(std::ctype_base::space)));
}

} // namespace lfjail

#endif	// LFJAIL_STRING_UTILS_HH
