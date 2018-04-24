[@bs.val] external __dirname : string = "";

let rec findProjectRoot = startDir : string => {
  let packageJson = Node.Path.join([|startDir, "package.json"|]);
  let nodeModules = Node.Path.join([|startDir, "node_modules"|]);
  Node.Fs.existsSync(packageJson) && Node.Fs.existsSync(nodeModules) ?
    startDir :
    {
      let parts = Js.String.split("/", startDir);
      let parentParts = Js.Array.slice(parts, ~start=0, ~end_=-1);
      findProjectRoot(Js.Array.joinWith("/", parentParts));
    };
};

let projectRoot = findProjectRoot(__dirname);