# Blender3D Bones File: a.bones
transl ROOT_transl
0.1014 -0.1976 3.3309
rot_quat Racine_rot
0.6869 0.7268 -0.0059 0.0038
bone Racine
0.8358
parent_bone NULL

  transl Racine_transl
  0 0.8358 0
  rot_quat Thoracique_rot
  0.9738 -0.2157 0.0682 -0.0243
  bone Thoracique
  0.8532
  parent_bone Racine

    transl Thoracique_transl
    0 0.8532 0
    rot_quat Tete_rot
    0.9655 0.2548 -0.0522 0.0101
    bone Tete
    0.8914
    parent_bone Thoracique

    bone_end Tete

    transl Thoracique_transl
    0 0.8532 0
    rot_quat Epaule_G_rot
    0.4729 -0.4032 -0.3992 -0.6741
    bone Epaule_G
    0.6007
    parent_bone Thoracique

      transl Epaule_G_transl
      0 0.6007 0
      rot_quat Bra_G_rot
      0.9192 -0.3553 0.1277 0.1122
      bone Bra_G
      0.7386
      parent_bone Epaule_G

        transl Bra_G_transl
        0 0.7386 0
        rot_quat AvantBra_G_rot
        0.9743 0.0406 -0.0006 -0.2217
        bone AvantBra_G
        0.7567
        parent_bone Bra_G

          transl AvantBra_G_transl
          0 0.7567 0
          rot_quat Main_G_rot
          0.996 -0.0019 0.0224 0.086
          bone Main_G
          0.4414
          parent_bone AvantBra_G

            transl Main_G_transl
            0 0.4414 0
            rot_quat Main_G.001_rot
            0.7629 0.0633 0.5616 -0.314
            bone Main_G.001
            0.2157
            parent_bone Main_G

              transl Main_G.001_transl
              0 0.2157 0
              rot_quat Main_G.002_rot
              0.4765 -0.4038 -0.7713 0.1223
              bone Main_G.002
              0.1809
              parent_bone Main_G.001

              bone_end Main_G.002

            bone_end Main_G.001

            transl Main_G_transl
            0 0.4414 0
            rot_quat Main_G.003_rot
            0.8596 -0.0357 -0.4242 0.2826
            bone Main_G.003
            0.1681
            parent_bone Main_G

              transl Main_G.003_transl
              0 0.1681 0
              rot_quat Main_G.004_rot
              0.7057 -0.3517 0.615 -0.0008
              bone Main_G.004
              0.1562
              parent_bone Main_G.003

              bone_end Main_G.004

            bone_end Main_G.003

          bone_end Main_G

        bone_end AvantBra_G

      bone_end Bra_G

    bone_end Epaule_G

    transl Thoracique_transl
    0 0.8532 0
    rot_quat Epaule_D_rot
    0.462 -0.5803 0.3951 0.542
    bone Epaule_D
    0.5466
    parent_bone Thoracique

      transl Epaule_D_transl
      0 0.5466 0
      rot_quat Bra_D_rot
      0.9122 -0.3271 -0.209 -0.1309
      bone Bra_D
      0.7559
      parent_bone Epaule_D

        transl Bra_D_transl
        0 0.7559 0
        rot_quat AvantBra_D_rot
        0.9868 0.006 -0.0443 0.1556
        bone AvantBra_D
        0.7235
        parent_bone Bra_D

          transl AvantBra_D_transl
          0 0.7235 0
          rot_quat Main_D_rot
          0.9939 0.015 -0.0896 -0.0621
          bone Main_D
          0.4627
          parent_bone AvantBra_D

            transl Main_D_transl
            0 0.4627 0
            rot_quat Main_D.001_rot
            0.9336 -0.1063 -0.0137 0.342
            bone Main_D.001
            0.1726
            parent_bone Main_D

              transl Main_D.001_transl
              0 0.1726 0
              rot_quat Main_D.003_rot
              0.9525 -0.2462 -0.0607 -0.1689
              bone Main_D.003
              0.1424
              parent_bone Main_D.001

              bone_end Main_D.003

            bone_end Main_D.001

            transl Main_D_transl
            0 0.4627 0
            rot_quat Main_D.002_rot
            0.7128 0.0252 0.4611 -0.5279
            bone Main_D.002
            0.1591
            parent_bone Main_D

              transl Main_D.002_transl
              0 0.1591 0
              rot_quat Main_D.004_rot
              0.65 -0.4392 -0.5648 0.256
              bone Main_D.004
              0.1494
              parent_bone Main_D.002

              bone_end Main_D.004

            bone_end Main_D.002

          bone_end Main_D

        bone_end AvantBra_D

      bone_end Bra_D

    bone_end Epaule_D

  bone_end Thoracique

bone_end Racine

transl ROOT_transl
0.1014 -0.1976 3.3309
rot_quat Hanche_D_rot
0.7355 -0.1441 -0.1197 0.6511
bone Hanche_D
0.3201
parent_bone NULL

  transl Hanche_D_transl
  0 0.3201 0
  rot_quat Femur_D_rot
  0.2138 -0.2463 -0.8048 0.496
  bone Femur_D
  1.3765
  parent_bone Hanche_D

    transl Femur_D_transl
    0 1.3765 0
    rot_quat Tibia_D_rot
    0.1774 -0.0577 -0.9824 -0.0135
    bone Tibia_D
    1.6442
    parent_bone Femur_D

      transl Tibia_D_transl
      0 1.6442 0
      rot_quat Pied_D_rot
      0.2589 0.1734 -0.7023 0.64
      bone Pied_D
      0.77
      parent_bone Tibia_D

      bone_end Pied_D

    bone_end Tibia_D

  bone_end Femur_D

bone_end Hanche_D

transl ROOT_transl
0.1014 -0.1976 3.3309
rot_quat Hanche_G_rot
0.7677 -0.0722 0.1364 -0.6219
bone Hanche_G
0.3364
parent_bone NULL

  transl Hanche_G_transl
  0 0.3364 0
  rot_quat Femur_G_rot
  0.0498 -0.0321 -0.8239 0.5637
  bone Femur_G
  1.3829
  parent_bone Hanche_G

    transl Femur_G_transl
    0 1.3829 0
    rot_quat Tibia_G_rot
    0.4708 -0.0279 0.8813 0.0283
    bone Tibia_G
    1.6598
    parent_bone Femur_G

      transl Tibia_G_transl
      0 1.6598 0
      rot_quat Pied_G_rot
      0.2505 0.2263 0.7061 -0.6225
      bone Pied_G
      0.7825
      parent_bone Tibia_G

      bone_end Pied_G

    bone_end Tibia_G

  bone_end Femur_G

bone_end Hanche_G

