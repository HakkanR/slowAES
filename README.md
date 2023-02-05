# slowAES (slowAES.decrypt(c, 2, a, b) bypass test cookie
 **slowAES** is a library that allows you to bypass **"test cookie"** used web pages that protected by **aes.js** to prevent web-scrapping/or systems can not run javascripts.

 ## What is slowAES web-scraping protection? ##
 When you get a web page with HttpClient object by nodemcu, your response like below instead of pageview in web browser:

 ```html
<html>
<body>
    <script type="text/javascript" src="/aes.js"></script>
    <script>
        function toNumbers(d) {
            var e = [];
            d.replace(/(..)/g, function(d) {
                e.push(parseInt(d, 16))
            });
            return e
        }

        function toHex() {
            for (var d = [], d = 1 == arguments.length && 
            arguments[0].constructor == Array ? arguments[0] : 
            arguments, e = "", f = 0; f < d.length; f++) e += 
            (16 > d[f] ? "0" : "") + d[f].toString(16);
            return e.toLowerCase()
        }
        var a = toNumbers("f655ba9d09a112d4968c63579db590b4"),
            b = toNumbers("98344c2eee86c3994890592585b49f80"),
            c = toNumbers("a76ce883a42eb6e9fff3e05d9cc6c7e5");
        document.cookie = "__test=" + toHex(slowAES.decrypt(c, 2, a, b)) + 
        "; expires=Thu, 31-Dec-37 23:55:55 GMT; path=/";
        location.href = "http://www.namazvakitleri.site/i=1";
    </script>
    <noscript>
        This site requires Javascript to work, please enable
Javascript in your browser or use a browser with Javascript support
</noscript>
</body>
</html>
```
This means the page you wanted to get has kind of **slowAES protection** based clients ip address. When you get the page with the browser, the code above generate a cookie called **__test** and redirect browser to the page you wanted with that cookie plus a query string i, attemtp etc.

ie.
the page you wanted to get : http://www.namazvakitleri.site

the page seen on browser : http://www.namazvakitleri.site/?i=1 

with a cookie named **__test** and valued : **de1a59425ae4ce0c74d68c19716ffe76**

this means when you try to get this page with HttpClient you have to add a header:

```c
http.addHeader("Cookie", "__test=a6f0d5704838a5ecc1f9fcdd54396a2f", true);
```
and get the page with querystring generated on your page like "**the page seen on browser**" example above.

 
 If you find any problem or bug on this library, please use [Issues](https://github.com/HakkanR/slowAES/issues) feature on github or contact me on my web page 
 
 [devrelerim.com](https://www.devrelerim.com/) **Turkish**
 
 [devrelerim.com](https://en.devrelerim.com/) **English**

 You can follow me on:
 * [Youtube](https://www.youtube.com/HakkanR)
 * [Twitter](https://www.twitter.com/HakkanR)
 * [Instagram](https://www.instagram.com/hakkan_r)
 * [FaceBook](https://www.facebook.com/HakkanR)
 
 ## Basic Usage
 include header file
 ```cpp
 #include <slowAES.h>
 ```
 Get the first page which contains jacascript code, parse your a,b,c values in response and set them to variables called a,b,c: 
 
```c
const char* a = "f655ba9d09a112d4968c63579db590b4";
const char* b = "98344c2eee86c3994890592585b49f80";
const char* c = "a76ce883a42eb6e9fff3e05d9cc6c7e5";
```
Convert them to uint8_t arrays: 
 
```c
uint8_t aNums[16];
uint8_t bNums[16];
uint8_t cNums[16];
toNumbers(a, aNums);
toNumbers(b, bNums);
toNumbers(c, cNums);
```

 Initiate the object and call decrypt **aNums**, **bNums**, **cNums**, arrays:
```c
uint8_t finalCookie[33];
uint8_t resNums[16];
slowAES _slowAES;
_slowAES.decrypt(cNums, aNums, bNums,resNums);
toHex(resNums, finalCookie);
```

Your Final cookie is ready as follows:
```c
String str = (char *)finalCookie;
String fullCookieStr = "__test=" + str;
```

> You can find a working example in examples folder.

## Download
You can download this on
* [Github](https://github.com/HakkanR/slowAES)
* Arduino Library Manager