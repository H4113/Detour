var Locale = "de";
var textLang = $(".lang");
var imgLang = $("img");
$.getJSON("lang.json", function(lang)
{
	for (var i = 0; i < textLang.length; i++) 
	{
		if (textLang[i].hasAttribute("data-lang"))
		{
			textLang[i].textContent = lang[textLang[i].getAttribute("data-lang")][Locale];
		}
	}
	for (var i = 0; i < imgLang.length; i++)
	{
		if (imgLang[i].hasAttribute("alt"))
		{
			imgLang[i].setAttribute("alt", lang[imgLang[i].getAttribute("alt")][Locale]);
		}
	}
});

