[%raw "global.fetch = require('node-fetch')"];

exception APIError(string);

let responseJsonOrError = resp =>
  Fetch.(
    Response.ok(resp) ?
      Response.json(resp) :
      raise(
        APIError(
          string_of_int(Response.status(resp)) ++ Response.statusText(resp),
        ),
      )
  );