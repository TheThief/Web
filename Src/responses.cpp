#include "../include/responses.h"

#include "../include/HTTPResponse.h"

char status301content[] =
	"<html>\r\n"
	"<body>\r\n"
	"<h1>301 Permenantly Moved</h1>\r\n"
	"<p>You probably missed a / off of a folder url. See the \"Location\" header for details.</p>\r\n"
	"</body>\r\n"
	"</html>\r\n";
const HTTPResponseHTML status301(301, "Permenantly Moved", sizeof(status301content)-1, status301content);

char status400content[] =
	"<html>\r\n"
	"<body>\r\n"
	"<h1>400 Bad Request</h1>\r\n"
	"<p>This is a <i>HTTP</i> server, idiot.</p>\r\n"
	"</body>\r\n"
	"</html>\r\n";
const HTTPResponseHTML status400(400, "Bad Request", sizeof(status400content)-1, status400content);

char status403content[] =
	"<html>\r\n"
	"<body>\r\n"
	"<h1>403 Forbidden</h1>\r\n"
	"<p>You're not allowed to access this...</p>\r\n"
	"</body>\r\n"
	"</html>\r\n";
const HTTPResponseHTML status403(403, "Forbidden", sizeof(status403content)-1, status403content);

char status404content[] =
	"<html>\r\n"
	"<body>\r\n"
	"<h1>404 File Not Found</h1>\r\n"
	"<p>Maybe if you tried for a file that actually existed you might have more luck :)</p>\r\n"
	"</body>\r\n"
	"</html>\r\n";
const HTTPResponseHTML status404(404, "File Not Found", sizeof(status404content)-1, status404content);

const char status500content[] =
	"<html>\r\n"
	"<body>\r\n"
	"<h1>500 Internal Server Error</h1>\r\n"
	"<p>Unknown error, probably an internal limitation.</p>\r\n"
	"</body>\r\n"
	"</html>\r\n";
const HTTPResponseHTML status500(500, "Internal Server Error", sizeof(status500content)-1, status500content);

const char status501content[] =
	"<html>\r\n"
	"<body>\r\n"
	"<h1>501 Not Implemented</h1>\r\n"
	"<p>This web server only supports GET requests.</p>\r\n"
	"</body>\r\n"
	"</html>\r\n";
const HTTPResponseHTML status501(501, "Not Implemented", sizeof(status501content)-1, status501content);

const char status505content[] =
	"<html>\r\n"
	"<body>\r\n"
	"<h1>505 HTTP Version Not Supported</h1>\r\n"
	"<p>This web server only supports HTTP/1.1 requests.</p>\r\n"
	"<p>If you are still using HTTP/1.0, Get with the times already.</p>\r\n"
	"</body>\r\n"
	"</html>\r\n";
const HTTPResponseHTML status505(505, "HTTP Version Not Supported", sizeof(status505content)-1, status505content);
