{
  description = "A very basic flake";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
  };

  outputs = { self, nixpkgs }: 
  let
    system = "x86_64-linux";
    pkgs = nixpkgs.legacyPackages.${system};
  in
  {
    devShells.${system}.default = pkgs.mkShell {
      shellHook = /* bash */ ''
        zsh
      '';

      packages = with pkgs; [
        python3
        screen
        arduino-cli
        picotool
        (writers.writeBashBin "flashrp2040" /* bash */ ''
          mkdir result
          arduino-cli compile -b rp2040:rp2040:adafruit_feather_rfm --build-path result
          picotool uf2 convert result/*.bin result/out.uf2

          sudo mount /dev/sd*1 /mnt
          sudo cp result/out.uf2 /mnt
          sudo umount -r /mnt
        '')
      ];
    };

    packages.${system}.default = pkgs.stdenv.mkDerivation {
      name = "fish";
      src = ./.;

      nativeBuildInputs = with pkgs; [ 
        python3
        arduino-cli
        picotool
      ];

      buildPhase = ''
        mkdir $out
        arduino-cli compile -b rp2040:rp2040:adafruit_feather_rfm --build-path $out
        picotool uf2 convert $out/*.bin $out/out.uf2
      '';
    };
  };
}
