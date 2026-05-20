#!/bin/bash
# ============================================================
#  demo.sh — Compile et teste les 5 programmes sur Fedora Server
#  Usage: bash demo.sh
# ============================================================

echo "========================================"
echo "  COMPILATION DES 5 PROGRAMMES"
echo "========================================"

gcc -Wall -Wextra attr.c    -o attr    && echo "[OK] attr"
gcc -Wall -Wextra cdroit.c  -o cdroit  && echo "[OK] cdroit"
gcc -Wall -Wextra chgroup.c -o chgroup && echo "[OK] chgroup"
gcc -Wall -Wextra chuser.c  -o chuser  && echo "[OK] chuser"
gcc -Wall -Wextra suppr.c   -o suppr   && echo "[OK] suppr"

echo ""
echo "========================================"
echo "  PREPARATION DU FICHIER DE TEST"
echo "========================================"

echo "hello world" > test.txt
chmod 644 test.txt
echo "[OK] test.txt cree avec les droits 644"
ls -l test.txt

echo ""
echo "========================================"
echo "  1. ATTR — Afficher les attributs"
echo "========================================"
./attr test.txt

echo ""
echo "========================================"
echo "  2. CDROIT — Ajouter execution au user"
echo "========================================"
./cdroit test.txt user execution
ls -l test.txt

echo ""
echo "========================================"
echo "  2b. CDROIT — Ajouter lecture au group"
echo "========================================"
./cdroit test.txt group lecture
ls -l test.txt

echo ""
echo "========================================"
echo "  2c. CDROIT — Enlever tous les droits other"
echo "========================================"
./cdroit test.txt other rien
ls -l test.txt

echo ""
echo "========================================"
echo "  3. CHGROUP — Changer le groupe"
echo "========================================"
echo "(les groupes disponibles sur ce systeme:)"
cut -d: -f1 /etc/group | head -10
echo "..."
echo ""
echo "On essaye avec le groupe 'zakaria':"
./chgroup test.txt zakaria
ls -l test.txt

echo ""
echo "========================================"
echo "  4. CHUSER — Changer le proprietaire"
echo "========================================"
echo "(necessite sudo)"
sudo ./chuser test.txt root
ls -l test.txt

echo ""
echo "========================================"
echo "  5. SUPPR — Supprimer le fichier"
echo "========================================"
./suppr test.txt
ls -l test.txt 2>&1

echo ""
echo "========================================"
echo "  TERMINE"
echo "========================================"
