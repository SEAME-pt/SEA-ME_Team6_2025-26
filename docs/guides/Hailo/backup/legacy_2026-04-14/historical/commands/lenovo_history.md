  700  cd hailo8_sdk/
  701  ls
  702  cd ~/hailo-dfc/hailo8_sdk
  703  tar -xzf hailo8_ai_sw_suite_2025-10.tar.gz
  704  cd ~/hailo-dfc/hailo8_sdk
  705  ls -lh
  706  tar -xvzf hailo8_ai_sw_suite_2025-10.tar.gz
  707  ls | grep version.py
  708  ls | grep "version.py"
  709  ls
  710  ~/hailo-dfc
  711  ls
  712  cd ..
  713  ls
  714  ~/hailo-dfc
  715  ls -lh
  716  rm Dockerfile
  717  rm docker-compose.yaml
  718  rm hailo_dataflow_compiler*.whl
  719  rm hailort*.deb
  720  ls -lh
  721  cd hailo8_sdk/
  722  tar -xzf hailo8_ai_sw_suite_2025-10.tar.gz
  723  ls
  724  docker load < hailo8_ai_sw_suite_2025-10.tar.gz
  725  ls 
  726  docker compose run dfc bash
  727  cd ..
  728  ls
  729  cd hailo8_sdk/
  730  ls
  731  docker compose run dfc bash
  732  cd shared_with_docker/
  733  ls
  734  cd shared_with_docker/
  735  docker compose run dfc bash
  736  ls
  737  cd ..
  738  ls
  739  cd ..
  740  cd workspace/
  741  ls
  742  cd hailo_model_zoo/
  743  ls
  744  cd ..
  745  ls
  746  cd ..
  747  ls
  748  cd hailo8_sdk/
  749  ls
  750  ./hailo_ai_sw_suite_docker_run.sh 
  751  ./hailo_ai_sw_suite_docker_run.sh --resume
  752  ./hailo_ai_sw_suite_docker_run.sh 
  753  clear
  754  ./hailo_ai_sw_suite_docker_run.sh --resume
  755  celar
  756  clear
  757  ./hailo_ai_sw_suite_docker_run.sh --resume
  758  sudo apt-get update
  759  sudo apt-get install -y ca-certificates curl gnupg
  760  sudo install -m 0755 -d /etc/apt/keyrings
  761  curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo gpg --dearmor -o /etc/apt/keyrings/docker.gpg
  762  sudo install -m 0755 -d /etc/apt/keyrings
  763  curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo gpg --dearmor -o /etc/apt/keyrings/docker.gpg
  764  sudo chmod a+r /etc/apt/keyrings/docker.gpg
  765  echo "deb [arch=$(dpkg --print-architecture) \
  766  signed-by=/etc/apt/keyrings/docker.gpg] \
  767  https://download.docker.com/linux/ubuntu \
  768  $(. /etc/os-release && echo "$VERSION_CODENAME") stable" | sudo tee /etc/apt/sources.list.d/docker.list
  769  sudo apt-get update
  770  sudo apt-get install -y docker-ce docker-ce-cli containerd.io docker-compose-plugin
  771  sudo usermod -aG docker $USER
  772  docker --version
  773  docker run hello-world
  774  docker --version
  775  docker run hello-world
  776  newgrp docker
  777  echo "PID:$! — A rodar em background. Monitorar com: tail -f /tmp/setup_live.log"
  778  PID=$!
  779  echo "✅ Script PID:$PID em background"
  780  echo "Para monitorar (abrir OUTRO TERMINAL e rodar):"
  781  echo "tail -f /tmp/setup_live.log"
  782  tail -f /tmp/setup_live.log
  783  cat /home/seame/Documents/SEA-ME_Team6_2025-26/docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT-L0-1.md
  784  echo "---"
  785  ls /home/seame/Documents/SEA-ME_Team6_2025-26/docs/TSF/tsf_implementation/.trudag_items/EVIDENCES/ | head -10
  786  echo "---"
  787  cat /home/seame/Documents/SEA-ME_Team6_2025-26/docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_1/EXPECT-L0-1.md
  788  cat "/home/seame/Documents/SEA-ME_Team6_2025-26/docs/TSF/tsf_implementation/.trudag_items/ASSUMPTIONS/ASSUMP_L0_1/ASSUMPTIONS-ASSUMP_L0_1.md"
  789  for i in 1 5 10 15 20 25 31; do   echo "=== EXPECT-L0-$i ===";   cat "/home/seame/Documents/SEA-ME_Team6_2025-26/docs/TSF/tsf_implementation/items/expectations/EXPECT-L0-$i.md" 2>/dev/null || echo "NOT FOUND";   echo; done
  790  echo "---"
  791  head -25 /home/seame/Documents/SEA-ME_Team6_2025-26/docs/TSF/tsf_implementation/items/expectations/EXPECT-L0-20.md
  792  echo "---"
  793  head -25 /home/seame/Documents/SEA-ME_Team6_2025-26/docs/TSF/tsf_implementation/items/expectations/EXPECT-L0-1.md
  794  grep -l "^evidence:" /home/seame/Documents/SEA-ME_Team6_2025-26/docs/TSF/tsf_implementation/items/expectations/*.md 2>/dev/null | wc -l
  795  echo "with evidence"
  796  ls /home/seame/Documents/SEA-ME_Team6_2025-26/docs/TSF/tsf_implementation/items/expectations/*.md | wc -l
  797  echo "total"
  798  grep -rl "^evidence:" /home/seame/Documents/SEA-ME_Team6_2025-26/docs/TSF/tsf_implementation/items/evidences/ | wc -l
  799  echo "with evidence"
  800  ls /home/seame/Documents/SEA-ME_Team6_2025-26/docs/TSF/tsf_implementation/items/evidences/*.md | wc -l
  801  echo "total evidences"
  802  # Check ASSERTIONS too
  803  grep -rl "^evidence:" /home/seame/Documents/SEA-ME_Team6_2025-26/docs/TSF/tsf_implementation/items/assertions/ | wc -l
  804  echo "assertions with evidence"
  805  echo "files with evidence block"
  806  # check working items from trudag
  807  grep "evidence:" /home/seame/Documents/SEA-ME_Team6_2025-26/docs/TSF/tsf_implementation/.trudag_items/ASSUMPTIONS/ASSUMP_L0_1/ASSUMPTIONS-ASSUMP_L0_1.md
  808  trudag score 2>&1 | grep "= 0.0"
  809  echo "==="
  810  head -30 /home/seame/Documents/SEA-ME_Team6_2025-26/docs/TSF/tsf_implementation/items/evidences/EVID-L0-22.md
  811  ls /home/seame/Documents/SEA-ME_Team6_2025-26/docs/TSF/tsf_implementation/TSF_docs/WhatsTSF.md 2>&1
  812  # And check EVID_L0_1's reference
  813  ls "/home/seame/Documents/SEA-ME_Team6_2025-26/docs/guides/Power Consumption Analysis.md" 2>&1
  814  stat /home/seame/Documents/SEA-ME_Team6_2025-26/docs/TSF/tsf_implementation/items/evidences/EVID-L0-14.md | grep Modify
  815  diff <(cat /home/seame/Documents/SEA-ME_Team6_2025-26/docs/TSF/tsf_implementation/.trudag_items/EVIDENCES/EVID_L0_14/EVIDENCES-EVID_L0_14.md) <(cat /home/seame/Documents/SEA-ME_Team6_2025-26/docs/TSF/tsf_implementation/items/evidences/EVID-L0-14.md)
  816  ls /home/seame/Documents/SEA-ME_Team6_2025-26/docs/TSF/tsf_implementation/.trudag.dot 2>/dev/null
  817  ls /home/seame/Documents/SEA-ME_Team6_2025-26/docs/TSF/tsf_implementation/.dotstop.dot 2>/dev/null
  818  grep -c "\->" /home/seame/Documents/SEA-ME_Team6_2025-26/docs/TSF/tsf_implementation/.dotstop.dot
  819  # Also check for a trudag config
  820  cat /home/seame/Documents/SEA-ME_Team6_2025-26/docs/TSF/tsf_implementation/.trudag.yml 2>/dev/null || echo "no .trudag.yml"
  821  sed -n '240,320p' /home/seame/Documents/SEA-ME_Team6_2025-26/docs/TSF/tsf_implementation/scripts/setup_trudag_clean.sh
  822  # Check if EXPECT-L0-22 item file exists
  823  ls /home/seame/Documents/SEA-ME_Team6_2025-26/docs/TSF/tsf_implementation/items/expectations/ | grep "22\|30\|31"
  824  echo "---"
  825  ls /home/seame/Documents/SEA-ME_Team6_2025-26/docs/TSF/tsf_implementation/items/evidences/ | grep -E "22|30|31"
  826  echo "---"
  827  ls /home/seame/Documents/SEA-ME_Team6_2025-26/docs/TSF/tsf_implementation/.trudag_items/EVIDENCES/ | grep -E "22|30|31"
  828  echo "---"
  829  ls /home/seame/Documents/SEA-ME_Team6_2025-26/docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ | grep -E "22|30|31"
  830  echo "---"
  831  ls /home/seame/Documents/SEA-ME_Team6_2025-26/docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ | tail -15
  832  ls /home/seame/Documents/SEA-ME_Team6_2025-26/docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ | grep "22"
  833  # Also check .dotstop.dot for the L0_22 node and links
  834  grep "L0.22\|30\|31" /home/seame/Documents/SEA-ME_Team6_2025-26/docs/TSF/tsf_implementation/.dotstop.dot | head -10
  835  grep "\->" /home/seame/Documents/SEA-ME_Team6_2025-26/docs/TSF/tsf_implementation/.dotstop.dot | grep -c "EXPECT"
  836  echo "total edges"
  837  grep "\->" /home/seame/Documents/SEA-ME_Team6_2025-26/docs/TSF/tsf_implementation/.dotstop.dot | grep "EXPECT" | wc -l
  838  echo "EXPECT edges"
  839  expect_edges=$(grep "\->" /home/seame/Documents/SEA-ME_Team6_2025-26/docs/TSF/tsf_implementation/.dotstop.dot | grep "EXPECT" | wc -l)
  840  echo "Total edges: $edges"
  841  echo "EXPECT edges: $expect_edges"
  842  trudag manage create-link "EXPECTATIONS-EXPECT_L0_21" "ASSUMPTIONS-ASSUMP_L0_21" 2>&1 | grep -v "shadows"
  843  trudag manage create-link "ASSERTIONS-ASSERT_L0_14" "EVIDENCES-EVID_L0_14" 2>&1 | grep -v "shadows"
  844  trudag manage create-link "ASSERTIONS-ASSERT_L0_31" "EVIDENCES-EVID_L0_31" 2>&1 | grep -v "shadows"
  845  echo "done sample"
  846  echo "exit: $?"
  847  trudag score 2>&1 | head -5
  848  grep "1\.0" /tmp/score_out.txt | wc -l && echo "perfect items"
  849  grep "0\.0" /tmp/score_out.txt
  850  grep "= 1\.0" /tmp/score_out.txt | wc -l
  851  # Check if lines with -> are long
  852  awk '/->/{print length($0), NR}' /home/seame/Documents/SEA-ME_Team6_2025-26/docs/TSF/tsf_implementation/.dotstop.dot | tail -5
  853  trudag manage create-link "EXPECTATIONS-EXPECT_L0_3" "ASSERTIONS-ASSERT_L0_3" 2>&1 | grep -v shadows
  854  echo "exit: $?"
  855  # Check if it was added
  856  tail -3 docs/TSF/tsf_implementation/.dotstop.dot
  857  wc -l docs/TSF/tsf_implementation/.dotstop.dot
  858  grep "EXPECT_L0_3" docs/TSF/tsf_implementation/.dotstop.dot | grep "\->"
  859  echo "DONE. Lines in .dotstop.dot: $(wc -l < docs/TSF/tsf_implementation/.dotstop.dot)"
  860  # Also find hidden db files at repo root
  861  find /home/seame/Documents/SEA-ME_Team6_2025-26/ -maxdepth 2 -name "*.db" -o -name "*.sqlite" 2>/dev/null
  862  awk 'NR>=80 && NR<=120' /home/seame/Documents/SEA-ME_Team6_2025-26/docs/TSF/tsf_implementation/.dotstop.dot
  863  create_link() {   local parent="$1";   local child="$2"
  864    trudag manage set-item "$parent" 2>&1 | grep -v "shadows";   trudag manage set-item "$child" 2>&1 | grep -v "shadows";   local result;   result=$(trudag manage create-link "$parent" "$child" 2>&1);   if echo "$result" | grep -q "already"; then     echo "SKIP (already linked): $parent -> $child";   else     echo "OK: $parent -> $child";   fi; }
  865  # ASSERT -> EVID missing  
  866  create_link "ASSERTIONS-ASSERT_L0_14" "EVIDENCES-EVID_L0_14"
  867  create_link "ASSERTIONS-ASSERT_L0_31" "EVIDENCES-EVID_L0_31"
  868  # EXPECT_L0_4 -> ASSUMP (ASSERT already done)
  869  create_link "EXPECTATIONS-EXPECT_L0_4"  "ASSUMPTIONS-ASSUMP_L0_4"
  870  # EXPECT_L0_5
  871  create_link "EXPECTATIONS-EXPECT_L0_5"  "ASSERTIONS-ASSERT_L0_5"
  872  # Remove the 4 stale L0_31 items (no edges to lose)
  873  for item in "ASSERTIONS-ASSERT_L0_31" "ASSUMPTIONS-ASSUMP_L0_31" "EVIDENCES-EVID_L0_31" "EXPECTATIONS-EXPECT_L0_31"; do   echo "Removing: $item";   trudag manage remove-item "$item" 2>&1 | grep -v shadows; done
  874  echo "Lines after remove: $(wc -l < .dotstop.dot)"
  875  cd /home/seame/Documents/SEA-ME_Team6_2025-26
  876  printf "REPO:%s\n" "$(basename "$(git rev-parse --show-toplevel)")"
  877  printf "CURRENT:%s\n" "$(git branch --show-current)"
  878  git status --porcelain
  879  printf "---BRANCHES---\n"
  880  echo "EXIT:$?"
  881  echo "PID:$!"
  882  clear
  883  source .venv/bin/activate && python3 /home/seame/Documents/SEA-ME_Team6_2025-26/docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py
  884  source .venv/bin/activate && docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --check
  885  source .venv/bin/activate && python docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --check
  886  source .venv/bin/activate && python docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --sync
  887  source .venv/bin/activate && python docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --validate
  888  trudag score --validate | grep "^EVIDENCES-EVID_L0_5"
  889  ls -l /tmp/tsf_validate_after_pathfix.txt && tail -n 120 /tmp/tsf_validate_after_pathfix.txt | cat
  890  if [ -x .venv/bin/trudag ]; then T=.venv/bin/trudag; elif [ -x docs/TSF/tsf_implementation/.venv/bin/trudag ]; then T=docs/TSF/tsf_implementation/.venv/bin/trudag; else T=$(command -v trudag); fi
  891  echo "TRUDAG=$T"
  892  "$T" --help | head -n 40 | cat
  893  echo '---'
  894  "$T" manage --help | head -n 80 | cat
  895  echo '---'
  896  "$T" manage lint; echo "manage_lint_exit=$?"
  897  source .venv/bin/activate && python docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --validate
  898  trudag score --validate | grep "^EVIDENCES-EVID_L0_5"
  899  ls -l /tmp/tsf_validate_after_pathfix.txt && tail -n 120 /tmp/tsf_validate_after_pathfix.txt | cat
  900  if [ -x .venv/bin/trudag ]; then T=.venv/bin/trudag; elif [ -x docs/TSF/tsf_implementation/.venv/bin/trudag ]; then T=docs/TSF/tsf_implementation/.venv/bin/trudag; else T=$(command -v trudag); fi
  901  echo "TRUDAG=$T"
  902  "$T" --help | head -n 40 | cat
  903  echo '---'
  904  "$T" manage --help | head -n 80 | cat
  905  echo '---'
  906  "$T" manage lint; echo "manage_lint_exit=$?"
  907  echo 1 | sudo tee /sys/class/backloght/intel_backlight/brightness
  908  echo 1 | sudo tee/sys/class/backloght/intel_backlight/brightness
  909  echo 1 | sudo tee /sys/class/backlight/intel_backlight/brightness
  910  ip a
  911  echo 19333 | sudo tee /sys/class/backloght/intel_backlight/brightness
  912  echo 19334 | sudo tee /sys/class/backloght/intel_backlight/brightness
  913  echo 19333 | sudo tee /sys/class/backloght/intel_backlight/brightness
  914  echo 3 | sudo tee /sys/class/backloght/intel_backlight/brightness
  915  echo 19333 | sudo tee /sys/class/backloght/intel_backlight/brightness
  916  echo 19333 | sudo tee /sys/class/backlight/intel_backlight/brightness
  917  speaker-test -t sine -f 440
  918  pavucontrol
  919  cd ~/shared_with_docker/
  920  ls
  921  sudo docker cp ~/Documents/yolo/yolov8n_simplified.onnx 262e73d13310:/local/workspace/hailo_model_zoo/hailo_models/custom_models/
  922  sudo docker exec -it 262e73d13310 bash
  923  source ./venv/bin/activate && python docs/TSF/tsf_implementation/scripts/setup_trudag_clean.sh
  924  source ./venv/bin/activate && python docs/TSF/tsf_implementation/scripts/setup_trudag_clean.sh --check
  925  source .venv/bin/activate && python docs/TSF/tsf_implementation/scripts/setup_trudag_clean.sh --check
  926  source .venv/bin/activate && python docs/TSF/tsf_implementation/scripts/setup_trudag_clean.sh 
  927  source .venv/bin/activate && docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --check
  928  clear
  929  source .venv/bin/activate && docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --check
  930  clear
  931  deactivate
  932  source .venv/bin/activate && python3 docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --check 
  933  source .venv/bin/activate && python3 docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --syb
  934  source .venv/bin/activate && python3 docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --sync
  935  source .venv/bin/activate && python docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --check
  936  source .venv/bin/activate && python docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --sync
  937  source .venv/bin/activate && python docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py 
  938  speaker-test -t sine -f 440
  939  pavucontrol
  940  systemctl --user restart pipewire pipewire-pulse
  941  aplay -l
  942  pavucontrol
  943  pulseaudio -k
  944  pulseaudio --start
  945  pactl set-default-sink alsa_output.pci-0000_00_1f.3.analog-stereo
  946  pactl list short sinks
  947  pactl set-default-sink alsa_output.pci-0000_00_1f.3.analog-stereo
  948  clear
  949  pactl set-default-sink alsa_output.pci-0000_00_1f.3-platform-skl_hda_dsp_generic.HiFi__hw_sofhdadsp__sink
  950  pavucontrol
  951  speaker-test -D alsa_output.pci-0000_00_1f.3-platform-skl_hda_dsp_generic.HiFi__hw_sofhdadsp__sink -t sine
  952  pavucontrol
  953  pactl list cards
  954  pactl set-card-profile alsa_card.pci-0000_00_1f.3 output:analog-stereo
  955  clear
  956  pactl set-card-profile alsa_card.pci-0000_00_1f.3-platform-skl_hda_dsp_generic HiFi
  957  pulseaudio -k
  958  pavucontrol
  959  pulseaudio -k
  960  pulseaudio --start
  961  pactl info
  962  pavucontrol
  963  pactl info
  964  systemctl --user status pulseaudio
  965  clear
  966  systemctl --user list-sockets | grep pulse
  967  rm -rf ~/.config/pulse
  968  rm -rf ~/.pulse
  969  pulseaudio --start
  970  pactl info
  971  pactl set-card-profile alsa_card.pci-0000_00_1f.3-platform-skl_hda_dsp_generic HiFi
  972  speaker-test -D default -t sine
  973  aplay /usr/share/sounds/alsa/Front_Center.wav
  974  pavucontrol
  975  pactl list short sinks
  976  clear
  977  pactl list sink-inputs
  978  pactl set-sink-port 3 Speaker
  979  pactl list sinks
  980  clear
  981  # Define o sink padrão como o HiFi com Speaker
  982  pactl set-default-sink alsa_output.pci-0000_00_1f.3-platform-skl_hda_dsp_generic.HiFi__hw_sofhdadsp__sink
  983  # Define o port ativo para Speaker
  984  pactl set-sink-port alsa_output.pci-0000_00_1f.3-platform-skl_hda_dsp_generic.HiFi__hw_sofhdadsp__sink analog-output-speaker
  985  pactl set-default-sink alsa_output.pci-0000_00_1f.3-platform-skl_hda_dsp_generic.HiFi__hw_sofhdadsp__sink
  986  pactl set-sink-port alsa_output.pci-0000_00_1f.3-platform-skl_hda_dsp_generic.HiFi__hw_sofhdadsp__sink analog-output-speaker
  987  speaker-test -D default -t sine
  988  clear
  989  pactl list sinks | grep -A 10 "Speaker \+ Headphones"
  990  pactl list sinks | grep -E "Name:|Ports:" -A 20
  991  clear
  992  pactl list sink-inputs
  993  pactl move-sink-input 0 alsa_output.pci-0000_00_1f.3-platform-skl_hda_dsp_generic.HiFi__hw_sofhdadsp__sink
  994  pavucontrol
  995  clear
  996  pactl list short sinks
  997  for i in $(pactl list short sink-inputs | cut -f1); do     pactl move-sink-input $i alsa_output.pci-0000_00_1f.3-platform-skl_hda_dsp_generic.HiFi__hw_sofhdadsp__sink; done
  998  source .venv/bin/activate && python docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --check
  999  source .venv/bin/activate && python docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --sync
 1000  source .venv/bin/activate && python docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --validate
 1001  source .venv/bin/activate && python docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --check
 1002  source .venv/bin/activate && python docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --sync
 1003  source .venv/bin/activate && python docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --validate
 1004  ls 
 1005  ls -lh
 1006  cd Desktop/ | ls
 1007  pwd
 1008  cd ..
 1009  ls -h
 1010  cd ..
 1011  ls -lh
 1012  pwd
 1013  cd /home/
 1014  ls
 1015  cd seame/
 1016  ls
 1017  ls -çlh
 1018  ls -lh
 1019  cd Documents/
 1020  ls -lh
 1021  cd AI
 1022  ls -lh
 1023  cd hailo/
 1024  ls -lh
 1025  cd hailo-dfc/
 1026  ls -lh
 1027  cd workspace/
 1028  ls -lh
 1029  cd ../../..
 1030  cd Docs/ | ls -lh
 1031  cd Docs
 1032  ls -lh
 1033  cd ../yolo/
 1034  ls -lh
 1035  cd ../..
 1036  cd CARLA/
 1037  ls -lh
 1038  cd ..
 1039  cd AI/hailo/Pre-Requisitos/packages/
 1040  ls -lh
 1041  cd ../../shared_with_docker/
 1042  ls -lh
 1043  cd doc/
 1044  ls -lh
 1045  cd ../../hailo-dfc/
 1046  ls -lh
 1047  cd workspace/
 1048  ls -lh
 1049  cd ../../hailo_models/ | ls -lh
 1050  cd ../../hailo_models/
 1051  ls -lh
 1052  cd ../..
 1053  cd ..
 1054  ls -lh
 1055  cd AI
 1056  cd hailo/Pre-Requisitos/packages/
 1057  ls -lh
 1058  clear
 1059  mkdir -p ~/hailo-dfc/docker_custom && cd ~/hailo-dfc/docker_custom
 1060  cp ../Pre-Requisitos/packages/hailo_dataflow_compiler-3.33.1-py3-none-linux_x86_64.whl .
 1061  cp ../Pre-Requisitos/packages/hailort_4.23.0_amd64.deb .
 1062  cp ../Pre-Requisitos/packages/hailo_dataflow_compiler-3.33.1-py3-none-linux_x86_64.whl .
 1063  clear
 1064  cp ~/Documents/AI/hailo/Pre-Requisitos/packages/hailo_dataflow_compiler-3.33.1-py3-none-linux_x86_64.whl .
 1065  cp ~/Documents/AI/hailo/Pre-Requisitos/packages/hailort_4.23.0_amd64.deb .
 1066  ls
 1067  mv ~/hailo-dfc/docker_custom ~/Documents/AI/hailo/
 1068  <<cd ~/Documents/AI/hailo/docker_custom
 1069  cd ~/Documents/AI/hailo/docker_custom
 1070  ls 
 1071  cd ..
 1072  ls
 1073  cd docker_custom/
 1074  ls
 1075  nano Dockerfile
 1076  cat Dockerfile 
 1077  sudo docker build -t hailo_custom_suite:latest .
 1078  nano Dockerfile
 1079  cd ~/Documents/AI/hailo/docker_custom
 1080  sudo docker build -t hailo_custom_suite:latest .
 1081  cat Dockerfile 
 1082  nano Dockerfile
 1083  sudo docker build -t hailo_custom_suite:latest .
 1084  sudo docker run -it --name hailo_custom_container   -v ~/Documents/AI/hailo/shared_with_docker:/local/workspace/shared_with_docker   hailo_custom_suite:latest
 1085  mkdir -p ~/Documents/AI/hailo/shared_with_docker/.hailomz/models_files/coco/2021-06-18/
 1086  cd ~/Documents/AI/hailo/shared_with_docker/.hailomz/models_files/coco/2021-06-18/
 1087  sudo docker run -it   -v ~/Documents/AI/hailo/shared_with_docker:/local/workspace/shared_with_docker   -v ~/Documents/AI/hailo/calibration_images:/local/workspace/hailo_model_zoo/hailo_models/custom_models/calibration_images   hailo_custom_suite:latest
 1088  coco_calib2017.tfrecord
 1089  wget <link_para_coco_calib2017.tfrecord>
 1090  wget https://hailo-public.s3.eu-central-1.amazonaws.com/datasets/coco/calib2017/coco_calib2017.tfrecord
 1091  clear
 1092  cd ..
 1093  ls
 1094  cd ..
 1095  mkdir -p COCO
 1096  cd COCO
 1097  wget http://images.cocodataset.org/zips/val2017.zip
 1098  unzip val2017.zip
 1099  cd ~/Documents/AI/hailo/shared_with_docker/COCO
 1100  wget http://images.cocodataset.org/annotations/annotations_trainval2017.zip
 1101  unzip annotations_trainval2017.zip
 1102  ls
 1103  cd val2017/
 1104  ls
 1105  cd ..
 1106  cd ~/Documents/AI/hailo/shared_with_docker/COCO
 1107  wget http://images.cocodataset.org/annotations/annotations_trainval2017.zip
 1108  unzip annotations_trainval2017.zip
 1109  ls
 1110  cd annotations/
 1111  ls- lh
 1112  ls -çh
 1113  ls -lh
 1114  cd ..
 1115  python3 ~/Documents/AI/hailo/hailo_model_zoo/hailo_model_zoo/datasets/create_coco_tfrecord.py     calib2017     --img_dir val2017     --ann_file annotations/instances_val2017.json     --output_name coco_calib2017.tfrecord
 1116  cd ..
 1117  ls
 1118  clear
 1119  cd shared_with_docker/
 1120  lks
 1121  ls
 1122  clear~
 1123  ls
 1124  mkdir -p ~/Documents/AI/hailo/shared_with_docker/scripts
 1125  cd ~/Documents/AI/hailo/shared_with_docker/scripts
 1126  ls
 1127  nano create_coco_calib_tfrecord.py
 1128  cat create_coco_calib_tfrecord.py 
 1129  ls
 1130  cd .
 1131  ls
 1132  cd ..
 1133  ls
 1134  ls -la
 1135  cd .hailomz/
 1136  ls
 1137  cd models_files/
 1138  ls
 1139  cd coco/
 1140  ls
 1141  cd 2021-06-18/
 1142  ls
 1143  clear
 1144  python3 ~/Documents/AI/hailo/shared_with_docker/scripts/create_coco_calib_tfrecord.py     ~/Documents/AI/hailo/shared_with_docker/COCO/val2017     --output_name ~/Documents/AI/hailo/shared_with_docker/.hailomz/models_files/coco/2021-06-18/coco_calib2017.tfrecord
 1145  python3 -m pip install --user tensorflow
 1146  python3 ~/Documents/AI/hailo/shared_with_docker/scripts/create_coco_calib_tfrecord.py     ~/Documents/AI/hailo/shared_with_docker/COCO/val2017     --output_name ~/Documents/AI/hailo/shared_with_docker/.hailomz/models_files/coco/2021-06-18/coco_calib2017.tfrecord
 1147  ls
 1148  sudo docker run -it   -v ~/Documents/AI/hailo/shared_with_docker:/local/workspace/shared_with_docker   -v ~/Documents/AI/hailo/calibration_images:/local/workspace/hailo_model_zoo/hailo_models/custom_models/calibration_images   hailo_custom_suite:latest
 1149  ls
 1150  cd Documents/
 1151  ls
 1152  cd AI/
 1153  ls
 1154  cd yolo/
 1155  ls
 1156  yolo export model=yolov8n.pt format=onnx opset=12 simplify=True dynamic=False
 1157  ls
 1158  cd ..
 1159  ls
 1160  cd hailo/
 1161  ls
 1162  cd docker_custom/
 1163  ls
 1164  cd ..
 1165  ls
 1166  cd shared_with_docker/
 1167  ls
 1168  yolo export model=yolov8n.pt format=onnx opset=12 simplify=True dynamic=False
 1169  clear
 1170  ~/Documents/AI/hailo/shared_with_docker
 1171  cd /Documents/AI/hailo/shared_with_docker
 1172  cd ..
 1173  cd hailo/
 1174  cd shared_with_docker/
 1175  yolo export model=yolov8n.pt format=onnx opset=12 simplify=True dynamic=False
 1176  docker ps
 1177  docker exec -it hailo_custom_suite:latest bash
 1178  ps -a
 1179  clear
 1180  docker exec -it interesting_nightingale bash
 1181  ls
 1182  cd ~/Documents/AI/hailo/shared_with_docker
 1183  yolo export model=yolov8n.pt format=onnx opset=12 simplify=True dynamic=False
 1184  docker exec -it interesting_nightingale bash
 1185  clear
 1186  docker ps -a
 1187  docker start interesting_nightingale
 1188  docker exec -it interesting_nightingale bash
 1189  cd Downloads/
 1190  ls
 1191  ssh root@10.21.220.191
 1192  ssh-keygen -f "/home/seame/.ssh/known_hosts" -R "10.21.220.191"
 1193  ssh root@10.21.220.191
 1194  clear
 1195  ssh root@10.21.220.191
 1196  cd Downloads/
 1197  ls
 1198  cp -r "$HOME/Documents/AI/hailo/calibration_images" "$HOME/Documents/AI/hailo/shared_with_docker/"
 1199  cd Documents/AI/hailo/shared_with_docker/
 1200  ls
 1201  ls ! grep calibration_images/
 1202  cd calibration_images/
 1203  ls
 1204  clear
 1205  cd Documents/
 1206  ls
 1207  cd AI/
 1208  ls
 1209  cd hailo/
 1210  ls
 1211  cd Pre-Requisitos/
 1212  ls
 1213  cd packages/
 1214  ls
 1215  clear
 1216  ssh root@10.21.220.191
 1217  docker run -it hailo_ai_sw_suite
 1218  docker ps
 1219  docker run -it  interesting_nightingale
 1220  docker run -it interesting_nightingale
 1221  docker run -it hailo_custom_suite:latest
 1222  cd Documents/AI/hailo/
 1223  ls
 1224  cd shared_with_docker/
 1225  ls
 1226  cat yolov8n_simplified.yaml 
 1227  claer
 1228  clear
 1229  docker run -it --rm   -v "$HOME/Documents/AI/hailo/shared_with_docker:/local/workspace/shared"   hailo_custom_suite:latest
 1230  clear
 1231  docker run -it --rm   -v "$HOME/Documents/AI/hailo/shared_with_docker:/local/workspace/shared"   -v "$HOME/Documents/AI/hailo/calibration_images:/local/workspace/calibration_images"   hailo_custom_suite:latest
 1232  docker run -it --rm   -v "$HOME/Documents/AI/hailo/shared_with_docker:/local/workspace/shared"   hailo_custom_suite:latest
 1233  clear
 1234  docker run -it --rm   -v "$HOME/Documents/AI/hailo/shared_with_docker:/local/workspace/shared"   hailo_custom_suite:latest
 1235  ls
 1236  cd calibration_images/
 1237  ls
 1238  cd ..
 1239  cd COCO/
 1240  ls
 1241  cd val2017/
 1242  wc -l
 1243  wc
 1244  clear
 1245  cd "$HOME/Documents/AI/hailo/shared_with_docker"
 1246  mkdir -p calibration_images
 1247  find COCO/val2017 -type f \( -iname "*.jpg" -o -iname "*.jpeg" -o -iname "*.png" \)   | head -n 256   | xargs -I{} cp "{}" calibration_images/
 1248  find calibration_images -type f | wc -l
 1249  ls
 1250  cd calibration_images/
 1251  ls
 1252  docker run -it --rm   -v "$HOME/Documents/AI/hailo/shared_with_docker:/local/workspace/shared"   hailo_custom_suite:latest
 1253  cd ..
 1254  ls
 1255  cd outputs/
 1256  ls
 1257  cd ..
 1258  cd scripts/
 1259  ls
 1260  cat create_coco_calib_tfrecord.py 
 1261  cd ..
 1262  ls
 1263  cd COCO
 1264  ls
 1265  cd ..
 1266  ls
 1267  cd ..
 1268  ls
 1269  cd hailo-dfc/
 1270  ls
 1271  cd workspace/
 1272  ls
 1273  cd hailo_model_zoo/
 1274  ls
 1275  cd docs
 1276  ls
 1277  cd ../hailo_model_zoo/
 1278  ls
 1279  cd ..
 1280  ls
 1281  cd training/
 1282  ls
 1283  cd ..
 1284  cd ,,
 1285  cd ..
 1286  ls
 1287  cd ..
 1288  ls
 1289  cd hailo8_sdk/
 1290  ls
 1291  cd shared_with_docker/
 1292  ls
 1293  cd doc/
 1294  cd ..
 1295  ls
 1296  cd ..
 1297  ls
 1298  cd hailo_models/
 1299  ls
 1300  cd ..
 1301  cd Pre-Requisitos/
 1302  ls
 1303  cd packages/
 1304  ls
 1305  cd ../../shared_with_docker/
 1306  ls
 1307  cd outputs/
 1308  cd script
 1309  cd ..
 1310  cd scripts/
 1311  ls
 1312  cd.. 
 1313  cd ..
 1314  cd COCO/
 1315  ls
 1316  clear
 1317  docker run -it --rm   -v "$HOME/Documents/AI/hailo/shared_with_docker:/local/workspace/shared"   hailo_custom_suite:latest
 1318  cd ..
 1319  ls
 1320  cd ..
 1321  ls
 1322  cd .. 
 1323  ls
 1324  clear
 1325  docker run -it --rm   -v "$HOME/Documents/AI/hailo/shared_with_docker:/local/workspace/shared"   hailo_custom_suite:latest
 1326  seame@seame6:~/Documents/AI$ docker run -it --rm   -v "$HOME/Documents/AI/hailo/shared_with_docker:/local/workspace/shared"   hailo_custom_suite:latest
 1327  Welcome to Hailo AI Software Suite Container
 1328  To list available commands, please type:
 1329  ----------------------------------------------------------------------
 1330  HailoRT:                                hailortcli -h
 1331  Dataflow Compiler:                      hailo -h
 1332  Hailo Model Zoo:                        hailomz -h
 1333  Run TAPPAS Detection Application:       tappas/detection/detection.sh
 1334  ----------------------------------------------------------------------
 1335  root@3eec75a4adce:/local/workspace# cd /local/
 1336  root@3eec75a4adce:/local# ls
 1337  workspace
 1338  root@3eec75a4adce:/local# cd workspace/
 1339  root@3eec75a4adce:/local/workspace# ls
 1340  doc  hailo_model_zoo  hailort_examples  hailo_virtualenv  shared  tappas
 1341  root@3eec75a4adce:/local/workspace# cd shared/
 1342  root@3eec75a4adce:/local/workspace/shared# ls
 1343  acceleras.log       compile_baseline.log  hailo_examples.log    outputs          scripts       yolov8n.pt
 1344  calibration_images  compile_yolov8n.log   hailort.log           pyhailort.log    yolov8n.har   yolov8n_simplified.onnx
 1345  COCO                doc                   hailo_sdk.client.log  run_onnx_cpu.py  yolov8n.onnx  yolov8n_simplified.yaml
 1346  root@3eec75a4adce:/local/workspace/shared# grep -E "Successful Compilation|HEF file written to|ERROR|Traceback" /local/workspace/shared/logs/compile_yolov8n_rebuild.log
 1347  tail -n 80 /local/workspace/shared/logs/compile_yolov8n_rebuild.log
 1348  find /local/workspace -maxdepth 6 -type f -name "yolov8n*.hef" | sort
 1349  grep: /local/workspace/shared/logs/compile_yolov8n_rebuild.log: No such file or directory
 1350  tail: cannot open '/local/workspace/shared/logs/compile_yolov8n_rebuild.log' for reading: No such file or directory
 1351  root@3eec75a4adce:/local/workspace/shared# 
 1352  clear
 1353  cd /local/workspace/shared
 1354  clear
 1355  docker run -it --rm   -v "$HOME/Documents/AI/hailo/shared_with_docker:/local/workspace/shared"   hailo_custom_suite:latest
 1356  ls
 1357  cd hailo/
 1358  ls
 1359  cd shared_with_docker/
 1360  ls
 1361  scp "$HOME/Documents/AI/hailo/shared_with_docker/yolov8n.hef" root@10.21.220.191:/root/models/
 1362  clear
 1363  scp "$HOME/Documents/AI/hailo/shared_with_docker/yolov8n.hef" root@10.21.220.191:/root/models/
 1364  ssh root@10.21.220.191 "mkdir -p /root/models && ls -ld /root/models"
 1365  scp -O "$HOME/Documents/AI/hailo/shared_with_docker/yolov8n.hef"   root@10.21.220.191:/data/yolov8n.hef
 1366  clear
 1367  ls -lh
 1368  cd AI
 1369  ls -lh
 1370  cd ..
 1371  cd CARLA
 1372  ls -lh
 1373  cd ../AI/
 1374  ls -lh
 1375  cd Docs/
 1376  ls -lh
 1377  cd ../hailo/
 1378  ls -lh
 1379  cd  calibration_images/
 1380  cd  ../docker_custom/
 1381  ls -lh
 1382  cd ../hailo-dfc/
 1383  ls -lh
 1384  cd ../hailo_models/
 1385  ls -lh
 1386  cd ../shared_with_docker/
 1387  ls -lh
 1388  docker run -it --rm   -v "$HOME/Documents/AI/hailo/shared_with_docker:/local/workspace/shared"   hailo_custom_suite:latest
 1389  lscpu
 1390  clear
 1391  nvidia-smi
 1392  git pull
 1393  git fetch origin
 1394  git pull
 1395  nvidia-sim
 1396  nvidia-smi
 1397  cd hailo-dfc/
 1398  ls
 1399  cd ../Documents/
 1400  ls -lh
 1401  cd AI/
 1402  ls -lh
 1403  cd Docs/
 1404  ls -lh
 1405  cd ../hailo/
 1406  ls -lh
 1407  cd calibration_images/
 1408  ls -lh
 1409  cd ../docker_custom/
 1410  ls -lh
 1411  cat Dockerfile 
 1412  cd ../hailo-dfc/
 1413  ls -lh
 1414  cd hailo8_sdk/
 1415  ls -lh
 1416  cd ../workspace/
 1417  ls -lh
 1418  cd ..
 1419  ls
 1420  cd workspace/hailo_model_zoo/
 1421  ls -lh
 1422  cd docs
 1423  ls -lh
 1424  cd ../hailo_model_zoo/
 1425  ls -lh
 1426  docker run -it --rm   -v "$HOME/Documents/AI/hailo/shared_with_docker:/local/workspace/shared"   hailo_custom_suite:latest
 1427  clear
 1428  ssh root@10.21.220.191
 1429  ssh-keygen -f "/home/seame/.ssh/known_hosts" -R "10.21.220.191"
 1430  ssh root@10.21.220.191
 1431  set -e
 1432  printf "\n[HOME candidate dirs]\n"
 1433  ls -d ~/hailo* ~/Documents/AI/hailo* 2>/dev/null || true
 1434  printf "\n[Likely Hailo folders up to depth 3]\n"
 1435  find ~ -maxdepth 3 -type d \( -iname "*hailo*" -o -iname "shared_with_docker" -o -iname "calibration_images" \) 2>/dev/null | sed "s|$HOME|~|" | sort
 1436  printf "\n[Python + packages]\n"
 1437  python3 --version || true
 1438  python3 -m pip list | grep -Ei "hailo|ultralytics|onnx" || true
 1439  printf "\n[Docker images hailo]\n"
 1440  docker images | grep -i hailo || true
 1441  printf "\n[Binaries]\n"
 1442  which hailomz || true
 1443  which hailortcli || true
 1444  clear
 1445  docker images | grep -i hailo
 1446  ls 
 1447  cd /Documents
 1448  cd Documents
 1449  clear
 1450  ls
 1451  clear
 1452  cd AI/
 1453  ls
 1454  cd Docs/
 1455  ls
 1456  cd ~/hailo-dfc/
 1457  ls
 1458  docker images | grep -i hailo
 1459  which hailortcli || true
 1460  which hailomz || true
 1461  ls
 1462  clear
 1463  cd .
 1464  cd ..
 1465  ls
 1466  cd Documents/
 1467  ls
 1468  cd AI/
 1469  cd hailo/
 1470  ls
 1471  cd calibration_images/
 1472  cd ../hailo-dfc/
 1473  ls
 1474  cd workspace/
 1475  ls
 1476  cd hailo_model_zoo/
 1477  ls
 1478  cd hailo_model_zoo/
 1479  ls
 1480  cd ..
 1481  cd..
 1482  cd ..
 1483  ls
 1484  cd shared_with_docker/
 1485  ls
 1486  cd calibration_images/
 1487  ls
 1488  clear
 1489  cd ..
 1490  ls
 1491  cd ..
 1492  clear
 1493  ls
 1494  cd Pre-Requisitos/
 1495  ls
 1496  cd packages/
 1497  ls
 1498  cd ..
 1499  cd .
 1500  cd ..
 1501  ls
 1502  clear
 1503  ls
 1504  cd docker_custom/
 1505  ls
 1506  cd --
 1507  ls
 1508  cd Documents/
 1509  clear
 1510  cd AI/
 1511  cd hailo/
 1512  ls
 1513  cd shared_with_docker/
 1514  ls
 1515  clear
 1516  cd ..
 1517  ls
 1518  clear
 1519  ls
 1520  cd hailo_models/
 1521  ls
 1522  cd ..
 1523  ls
 1524  clear
 1525  cd hailo-dfc/
 1526  ls
 1527  cd hailo8_sdk/
 1528  ls
 1529  cd shared_with_docker/
 1530  ls
 1531  cd doc
 1532  ls
 1533  clear
 1534  ccd ..
 1535  cd ..
 1536  ls
 1537  clear
 1538  ls
 1539  cat repositories 
 1540  ls
 1541  cd ..
 1542  cd .. 
 1543  ls
 1544  clear
 1545  ls
 1546  cd ..
 1547  ls
 1548  clear
 1549  ls
 1550  cd yolo/
 1551  ls
 1552  cd ../Docs/
 1553  ls
 1554  cd ..
 1555  ls
 1556  cd hailo/
 1557  ls
 1558  clear
 1559  cd hailo-dfc/
 1560  ls
 1561  cd hailo8_sdk/
 1562  ls
 1563  cd ../workspace/
 1564  ls
 1565  cd hailo_model_zoo/
 1566  ls
 1567  clear
 1568  cd ..
 1569  clear
 1570  ls
 1571  cd Docs/
 1572  ls
 1573  cd ../hailo/
 1574  ls
 1575  clear
 1576  ls
 1577  cd ..
 1578  cd AI/
 1579  ls
 1580  cd hailo/
 1581  ls
 1582  cd calibration_images/
 1583  ls
 1584  cd ../shared_with_docker/
 1585  ls
 1586  cd calibration_images/
 1587  cd ..
 1588  ls
 1589  cler
 1590  clear
 1591  # 1) Criar arquivo de legado (sem apagar já)
 1592  mkdir -p ~/Documents/AI/_archive_hailo_legacy
 1593  # 2) Mover vazios/legado óbvio
 1594  mv ~/hailo-dfc ~/Documents/AI/_archive_hailo_legacy/ 2>/dev/null || true
 1595  mv ~/Documents/AI/hailo/calibration_images ~/Documents/AI/_archive_hailo_legacy/calibration_images_empty 2>/dev/null || true
 1596  mv ~/Documents/AI/hailo/hailo_models ~/Documents/AI/_archive_hailo_legacy/hailo_models_empty 2>/dev/null || true
 1597  # 3) Normalizar packages
 1598  mkdir -p ~/Documents/AI/hailo_sdk_packages
 1599  mv ~/Documents/AI/hailo/Pre-Requisitos/packages/* ~/Documents/AI/hailo_sdk_packages/ 2>/dev/null || true
 1600  # 4) Padronizar execução Docker para a sprint
 1601  export HAILO_ROOT=~/Documents/AI/hailo
 1602  docker run -it   -v "$HAILO_ROOT/shared_with_docker:/local/workspace/shared"   hailo_custom_suite:latest bash
 1603  cd ..
 1604  ls
 1605  cd AI/
 1606  ls
 1607  cd hailo
 1608  ls
 1609  cd calibration_images/
 1610  ls
 1611  cd ../shared_with_docker/
 1612  ls
 1613  clear
 1614  tail -n 50 -f /home/seame/Documents/AI/hailo/shared_with_docker/logs/compile_yolov8n_seg_fallback.log
 1615  docker image inspect hailo_custom_suite:latest --format '{{.Architecture}}/{{.Os}}'
 1616  uname -m
 1617  docker run --rm --entrypoint /bin/sh hailo_custom_suite:latest -c 'echo ok'
 1618  docker run --rm --entrypoint /bin/bash hailo_custom_suite:latest -lc 'echo ok'
 1619  clear
 1620  export HAILO_ROOT=~/Documents/AI/hailo
 1621  docker run --rm -it   --entrypoint /bin/bash   -v "$HAILO_ROOT/shared_with_docker:/local/workspace/shared"   hailo_custom_suite:latest
 1622  cd Documents/
 1623  cd AI
 1624  cd hailo
 1625  ls
 1626  cd shared_with_docker/
 1627  ls
 1628  clear
 1629  cd ~/Documents/AI/hailo/shared_with_docker
 1630  python3 -m pip install -U pip ultralytics onnx
 1631  python3 -c "from ultralytics import YOLO; YOLO('yolo26n-seg.pt'); YOLO('yolo26s-seg.pt')"
 1632  python3 -c "from ultralytics import YOLO; YOLO('yolo26n-seg.pt').export(format='onnx')"
 1633  python3 -c "from ultralytics import YOLO; YOLO('yolo26s-seg.pt').export(format='onnx')"
 1634  ls -lh yolo26*.pt yolo26*.onnx
 1635  ls
 1636  export HAILO_ROOT=~/Documents/AI/hailo
 1637  docker run --rm -it --entrypoint /bin/bash   -v "$HAILO_ROOT/shared_with_docker:/local/workspace/shared"   hailo_custom_suite:latest
 1638  cler
 1639  clear
 1640  docker run --rm -it --entrypoint /bin/bash   -v "$HAILO_ROOT/shared_with_docker:/local/workspace/shared"   hailo_custom_suite:latest
 1641  docker ps
 1642  docker exec -it 290e0d3e6333 bash
 1643  clear
 1644  docker exec -it 290e0d3e6333 bash
 1645  docker run -it   -v "$HAILO_ROOT/shared_with_docker:/local/workspace/shared"   hailo_custom_suite:latest bash
 1646  docker exec -it 290e0d3e6333 bash
 1647  uname -m
 1648  docker images --digests | grep -i hailo_custom_suite
 1649  docker image inspect hailo_custom_suite:latest --format '{{.Architecture}}/{{.Os}}'
 1650  docker ps -a --format 'table {{.ID}}\t{{.Image}}\t{{.Status}}\t{{.Names}}'
 1651  clear
 1652  docker ps
 1653  docker exec -it interesting_nightingale /bin/bash
 1654  clear
 1655  # 1) Lenovo host: confirmar mount e procurar no disco
 1656  echo "$HAILO_ROOT"
 1657  ls -lah "$HAILO_ROOT/shared_with_docker" | cat
 1658  find "$HAILO_ROOT" -type f \( -name "yolov8n_seg.hef" -o -name "yolov8n_seg.har" -o -name "compile_yolov8n_seg_fallback.log" \) 2>/dev/null | cat
 1659  clear
 1660  # 1) Ver de onde o container UP está a montar /local/workspace/shared
 1661  docker inspect interesting_nightingale   --format '{{range .Mounts}}{{println .Source "->" .Destination}}{{end}}'
 1662  ls -lah "<SOURCE_DO_INSPECT>" | cat
 1663  find "<SOURCE_DO_INSPECT>" -maxdepth 2 -type f -name "yolov8n_seg.hef" -o -name "yolov8n_seg.har" -o -name "compile_yolov8n_seg_fallback.log" | cat
 1664  mkdir -p "$HOME/hailo_shared/logs"
 1665  docker run --rm -it --name hailo_compile_fix   -v "$HOME/hailo_shared:/local/workspace/shared"   hailo_custom_suite:latest /bin/bash
 1666  clear
 1667  # Lenovo host
 1668  docker exec -it interesting_nightingale /bin/bash
 1669  clear
 1670  # Lenovo host
 1671  export HAILO_SHARED="/home/seame/Documents/AI/hailo/shared_with_docker"
 1672  mkdir -p "$HAILO_SHARED"/{logs,results,hef,calibration_images}
 1673  # Entrar no container que já funciona
 1674  docker exec -it interesting_nightingale /bin/bash
 1675  # Lenovo host (fora do container): confirmar ficheiros persistidos
 1676  ls -lh /home/seame/Documents/AI/hailo/shared_with_docker/yolov8n_seg.hef
 1677  clear
 1678  scp /home/seame/Documents/AI/hailo/shared_with_docker/yolov8n_seg.hef root@10.21.220.191:/data/
 1679  ssh root@10.21.220.191
 1680  docker exec -it interesting_nightingale /bin/bash
 1681  clear
 1682  # no container Lenovo (interesting_nightingale)
 1683  docker exec -it interesting_nightingale /bin/bash
 1684  # Lenovo host (fora do container)
 1685  cd /home/seame/Documents/AI/hailo/shared_with_docker
 1686  python3 - << 'PY'
from ultralytics import YOLO
m = YOLO("yolo26n-seg.pt")
m.export(format="onnx", imgsz=512, opset=11, dynamic=False, simplify=True)
PY

 1687  mv -f yolo26n-seg.onnx yolo26n-seg_512.onnx
 1688  # container Lenovo
 1689  docker exec -it interesting_nightingale /bin/bash
 1690  python3 - << 'PY'
import onnx
m=onnx.load('/local/workspace/shared_with_docker/yolo26n-seg_512.onnx')
print([i.name for i in m.graph.input])
print([o.name for o in m.graph.output])
PY

 1691  clear
 1692  root@d61610b30f6f:/local/workspace# # no alvo (AGL/RPi)
 1693  hailortcli scan
 1694  Hailo devices not found
 1695  root@d61610b30f6f:/local/workspace# 
 1696  clear
 1697  docker exec -it interesting_nightingale /bin/bash
 1698  clear
 1699  history
seame@seame6:~/Documents/AI/hailo/shared_with_docker$ 
