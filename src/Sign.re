type hmac;

[@bs.module "crypto"] external createHmac : (string, string) => 'hmac = "";

[@bs.send] external update : (hmac, string) => unit = "";

[@bs.send] external digest : (hmac, Js.Nullable.t(string)) => string = "";

let signHmacSha256 = (message: string, secret: string) => {
  let signed = createHmac("sha256", secret);
  update(signed, message);
  digest(signed, Js.Nullable.return("hex"));
};