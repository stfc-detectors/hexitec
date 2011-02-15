#shared
lib_LTLIBRARIES = liblaue.la

#non shared
#lib_LIBRARIES = liblaue.a

liblaue_la_LDFLAGS = -version-info 7:0:0

liblaue_la_SOURCES =  \
decsym.f                ldm_ndset_pl.f          lfn_saint.f \
iminv.f                 ldm_newflp.f            lfn_saint_bdat.f \
ksm_asuset.f            ldm_newint.f            lfn_saint_df.f \
ksm_bdat.f              ldm_newstr.f            lfn_show_xdl.f \
ksm_centric.f           ldm_opfun.f             lfn_showref.f \
ksm_ch_reset.f          ldm_parse.f             lfn_soft.f \
ksm_get_nsym.f          ldm_parseitem.f         lfn_soft_df.f \
ksm_get_spgrp.f         ldm_parsekdm.f          lfn_soft_err.f \
ksm_get_sylat.f         ldm_pkmax.f             lfn_soft_list.f \
ksm_get_symm.f          ldm_plmax.f             lfn_soft_mon.f \
ksm_intfp.f             ldm_read_symm.f         lfn_soft_prog.f \
ksm_read_symm.f         ldm_readline.f          lfn_spot_cg.f \
ksm_reset.f             ldm_readstr.f           lfn_spot_rd.f \
ksm_set_symm.f          ldm_reset.f             lfn_spots_ana_lis.f \
ksm_sym_lirl.f          ldm_restrp.f            lfn_spots_analyse.f \
ksm_symfr2.f            ldm_rtod.f              lfn_unq_bdat.f \
ksm_symm_changed.f      ldm_rtod_dp.f           lfn_unq_gen.f \
ksm_symm_list.f         ldm_same_pk.f           lfn_unq_get.f \
ksm_symm_str.f          ldm_same_pl.f           lfn_unq_hkl.f \
ksm_sysabs.f            ldm_same_spd.f          lfn_unq_inc.f \
ksm_sysahk.f            ldm_saverp.f            lfn_unq_lims.f \
ksm_writesymm.f         ldm_set.f               lfn_unq_reset.f \
kwd_chk_parsekdm.f      ldm_set_a.f             lfn_unq_stat.f \
kwd_chk_parsesymm.f     ldm_set_alpha.f         lfn_unq_tab1.f \
kwd_opfun.f             ldm_set_axord.f         lfn_unq_tab2.f \
kwd_parseitem.f         ldm_set_b.f             lfn_unq_tab3.f \
kwd_parsekdm.f          ldm_set_base.f          lrl_a_calc.f \
kwd_parsesymm.f         ldm_set_beam.f          lrl_bdat.f \
kwd_readline.f          ldm_set_beta.f          lrl_calc_pos.f \
kwd_readstr.f           ldm_set_bulge.f         lrl_clr_all.f \
kwd_writekdm.f          ldm_set_c.f             lrl_clr_int.f \
kwd_writekdm_symm.f     ldm_set_cryst.f         lrl_counts.f \
ldm_addout.f            ldm_set_ctof.f          lrl_gen.f \
ldm_any_ch.f            ldm_set_dgeom.f         lrl_get.f \
ldm_asuset.f            ldm_set_dmin.f          lrl_get_harm.f \
ldm_axord.f             ldm_set_dspin.f         lrl_get_int.f \
ldm_bdat.f              ldm_set_dstor.f         lrl_get_nidx.f \
ldm_ch_reset.f          ldm_set_dtilt.f         lrl_get_ovlp.f \
ldm_changed.f           ldm_set_eps.f           lrl_get_ovpix.f \
ldm_chk_gen.f           ldm_set_fbox.f          lrl_get_plate.f \
ldm_chk_gpk.f           ldm_set_fidt.f          lrl_get_reass.f \
ldm_chk_gpl.f           ldm_set_fidx1.f         lrl_get_xfyfd.f \
ldm_chk_parse.f         ldm_set_fidx2.f         lrl_get_xycen.f \
ldm_chk_spk.f           ldm_set_fidx3.f         lrl_gethkl.f \
ldm_chk_spl.f           ldm_set_fidy1.f         lrl_getmult.f \
ldm_clr_gen.f           ldm_set_fidy2.f         lrl_gnr.f \
ldm_corr.f              ldm_set_fidy3.f         lrl_hcf.f \
ldm_dcset_pl.f          ldm_set_fname.f         lrl_mk_orient.f \
ldm_df_pid.f            ldm_set_fthr.f          lrl_nodidx.f \
ldm_df_spin.f           ldm_set_g1od.f          lrl_nodmax.f \
ldm_dfset_pk.f          ldm_set_gamma.f         lrl_numspots.f \
ldm_dfset_pl.f          ldm_set_imdat.f         lrl_ovall.f \
ldm_diset_pk.f          ldm_set_imtyp.f         lrl_ovlp.f \
ldm_diset_pl.f          ldm_set_keyp.f          lrl_ovm.f \
ldm_dtoqax.f            ldm_set_latt.f          lrl_ovq.f \
ldm_dtor.f              ldm_set_lmax.f          lrl_reass_cl.f \
ldm_dummy_chf.f         ldm_set_lmin.f          lrl_reass_ov.f \
ldm_dummy_chi.f         ldm_set_n1od.f          lrl_reassign.f \
ldm_dummy_chs.f         ldm_set_nonl.f          lrl_reccel.f \
ldm_err_i.f             ldm_set_nplat.f         lrl_set_clos.f \
ldm_err_neg.f           ldm_set_nump.f          lrl_set_idone.f \
ldm_err_r.f             ldm_set_nxras.f         lrl_set_int.f \
ldm_extend_init.f       ldm_set_nyras.f         lrl_set_oknod.f \
ldm_filename.f          ldm_set_ovpix.f         lrl_set_ovpix.f \
ldm_fval.f              ldm_set_phix.f          lrl_set_spov.f \
ldm_get.f               ldm_set_phiy.f          lrl_sort.f \
ldm_get_a.f             ldm_set_phiz.f          lrl_status.f \
ldm_get_alpha.f         ldm_set_pid.f           lrl_stillok.f \
ldm_get_axord.f         ldm_set_rast.f          lrl_tries.f \
ldm_get_b.f             ldm_set_rfl.f           lrl_xdlgnm.f \
ldm_get_base.f          ldm_set_rmax.f          lrl_xdlsim.f \
ldm_get_beam.f          ldm_set_rmin.f          lsc_abssurface_plot.f \
ldm_get_beta.f          ldm_set_roff.f          lsc_analyses.f \
ldm_get_bulge.f         ldm_set_rot.f           lsc_colours.f \
ldm_get_c.f             ldm_set_spac.f          lsc_deconv_logoutput.f \
ldm_get_cryst.f         ldm_set_spdx.f          lsc_file_output.f \
ldm_get_ctof.f          ldm_set_spdxn.f         lsc_get_outputflags.f \
ldm_get_dgeom.f         ldm_set_spdxy.f         lsc_graphsetup.f \
ldm_get_dmin.f          ldm_set_spdy.f          lsc_hist.f \
ldm_get_dspin.f         ldm_set_spdyn.f         lsc_hist_int.f \
ldm_get_dstor.f         ldm_set_spin.f          lsc_hist_lambda.f \
ldm_get_dtilt.f         ldm_set_spotb.f         lsc_hist_res.f \
ldm_get_eps.f           ldm_set_spotd.f         lsc_hist_theta.f \
ldm_get_fbox.f          ldm_set_spotf.f         lsc_irange.f \
ldm_get_fidt.f          ldm_set_spotl.f         lsc_menu1.f \
ldm_get_fidx1.f         ldm_set_spotw.f         lsc_menu1abs.f \
ldm_get_fidx2.f         ldm_set_sthr.f          lsc_menu2.f \
ldm_get_fidx3.f         ldm_set_sxmax.f         lsc_menu2abs.f \
ldm_get_fidy1.f         ldm_set_sxmin.f         lsc_menu3.f \
ldm_get_fidy2.f         ldm_set_symax.f         lsc_menu3abs.f \
ldm_get_fidy3.f         ldm_set_symin.f         lsc_mult_analog.f \
ldm_get_fname.f         ldm_set_symm.f          lsc_mult_analyses.f \
ldm_get_fthr.f          ldm_set_syst.f          lsc_next_multrefln.f \
ldm_get_g1od.f          ldm_set_templ.f         lsc_refset_analyses.f \
ldm_get_gamma.f         ldm_set_tilt.f          lsc_rfacts.f \
ldm_get_imdat.f         ldm_set_title.f         lsc_rfacts_lsp.f \
ldm_get_imtyp.f         ldm_set_toff.f          lsc_rfacts_out.f \
ldm_get_keyp.f          ldm_set_twist.f         lsc_rfout.f \
ldm_get_latt.f          ldm_set_wc.f            lsc_rout.f \
ldm_get_lmax.f          ldm_set_xc.f            lsc_scalestat_out.f \
ldm_get_lmin.f          ldm_set_xcenf.f         lsc_scatter.f \
ldm_get_n1od.f          ldm_set_xhigh.f         lsm_absc_mu.f \
ldm_get_nonl.f          ldm_set_xlow.f          lsm_abscor_rfn.f \
ldm_get_nplat.f         ldm_set_yc.f            lsm_abscrange.f \
ldm_get_nsym.f          ldm_set_ycenf.f         lsm_absfacglob.f \
ldm_get_nump.f          ldm_set_yhigh.f         lsm_absfacloc.f \
ldm_get_nxras.f         ldm_set_ylow.f          lsm_absopt.f \
ldm_get_nyras.f         ldm_set_yscal.f         lsm_absopt_lsp.f \
ldm_get_ovpix.f         ldm_setf_pk.f           lsm_add_pkpl.f \
ldm_get_phix.f          ldm_setf_pl.f           lsm_add_rflag.f \
ldm_get_phiy.f          ldm_seti_pk.f           lsm_anorm.f \
ldm_get_phiz.f          ldm_spd_par.f           lsm_calc_presets.f \
ldm_get_pid.f           ldm_spdmax.f            lsm_calcscfac.f \
ldm_get_rast.f          ldm_spdxy.f             lsm_calfun.f \
ldm_get_rfl.f           ldm_symfr2.f            lsm_calfun_absloc.f \
ldm_get_rmax.f          ldm_symm_list.f         lsm_cheb_1d_val.f \
ldm_get_rmin.f          ldm_symm_str.f          lsm_cheb_2d_val.f \
ldm_get_roff.f          ldm_sysabs.f            lsm_chebval.f \
ldm_get_rot.f           ldm_sysahk.f            lsm_check_mu.f \
ldm_get_spac.f          ldm_wrf_pk.f            lsm_clr_pkpl.f \
ldm_get_spdx.f          ldm_wrf_pl.f            lsm_clr_rflag.f \
ldm_get_spdxn.f         ldm_wri_pk.f            lsm_eqnsol.f \
ldm_get_spdxy.f         ldm_wri_pl.f            lsm_four_val.f \
ldm_get_spdy.f          ldm_write.f             lsm_indx.f \
ldm_get_spdyn.f         lfn_addobs.f            lsm_kdmcopy.f \
ldm_get_spgrp.f         lfn_auto_refn.f         lsm_keysetup.f \
ldm_get_spin.f          lfn_auto_soft.f         lsm_lambdalimits.f \
ldm_get_spotb.f         lfn_auto_upd.f          lsm_maxpkpl.f \
ldm_get_spotd.f         lfn_auto_updt.f         lsm_merge_plates.f \
ldm_get_spotf.f         lfn_calfun.f            lsm_missing_rflags.f \
ldm_get_spotl.f         lfn_clrobs.f            lsm_multfun_f.f \
ldm_get_spotw.f         lfn_detpro.f            lsm_multfun_sig.f \
ldm_get_sthr.f          lfn_dummy_cncl.f        lsm_multiples_deconv.f \
ldm_get_sxmax.f         lfn_findspots.f         lsm_next_refln.f \
ldm_get_sxmin.f         lfn_gelirl.f            lsm_next_refobs.f \
ldm_get_symax.f         lfn_get_intspot.f       lsm_norm_range.f \
ldm_get_symin.f         lfn_get_profile.f       lsm_num_refobs.f \
ldm_get_symm.f          lfn_get_profmax.f       lsm_omit.f \
ldm_get_syst.f          lfn_get_profpars.f      lsm_omit_lsp.f \
ldm_get_templ.f         lfn_get_profpix.f       lsm_parsetup.f \
ldm_get_tilt.f          lfn_get_spotcode.f      lsm_parsetup_ldm.f \
ldm_get_title.f         lfn_get_spotpars.f      lsm_parsetup_lsp.f \
ldm_get_toff.f          lfn_get_spotpix.f       lsm_refine.f \
ldm_get_twist.f         lfn_get_spots.f         lsm_scfac.f \
ldm_get_wc.f            lfn_getobs.f            lsm_scp_x.f \
ldm_get_xc.f            lfn_img_rect.f          lsm_sel_restore.f \
ldm_get_xcenf.f         lfn_int_stats1.f        lsm_sel_save.f \
ldm_get_xhigh.f         lfn_int_tab1.f          lsm_select_pkpl.f \
ldm_get_xlow.f          lfn_lirlge.f            lsm_select_screfs.f \
ldm_get_yc.f            lfn_list_intspot.f      lsm_select_screfs_lsp.f  \
ldm_get_ycenf.f         lfn_list_profs.f        lsm_set_rflags.f  \
ldm_get_yhigh.f         lfn_mtzlirl.f           lsm_setlsp.f \
ldm_get_ylow.f          lfn_nodhist.f           lsm_test_refobs.f \
ldm_get_yscal.f         lfn_numobs.f            lsm_top_lirl.f \
ldm_intfp.f             lfn_od_table.f          lsm_top_mults_lirl.f \
ldm_ival.f              lfn_plfit.f \
ldm_kdx.f               lfn_plfsub.f            syminv.f \
ldm_lirlsymm.f          lfn_refn_err.f          symmat.f \
ldm_lrl_calc.f          lfn_refn_log.f          turn3.f \
ldm_lwsymm.f            lfn_rfn.f               turnip.f \
ldm_mmrast.f            lfn_rfn_flgs.f \
ldm_ndset_pk.f          lfn_rotate_xy.f \
flmopn.f                filmrd.f                flmcls.f \
flmpos.f                flmrcl.f                filmrr.f \
dnls1.f                 dnls1e.f                \
dckder.f                denorm.f                dfdjc3.f \
dmpar.f                 dqrfac.f                dwupdt.f \
xermsg.f                dqrslv.f                fdump.f \
xerprn.f                xersve.f                xerhlt.f \
xercnt.f                xgetua.f                j4save.f \
i1mach.f                dcov.f                  minfit.f \
pythag.f                impopn.f                implrd.f \
impcls.f                imppos.f                imprcl.f \
implrr.f                flpval.f                getopt.f \
nxttok.f                strtkn.f                strval.f \
filext.f                indxiv.f                indxrv.f \
idxrnk.f                lsqmin.f                invmat.f \
ssqmin.f                lminls.f                fncalc.f \
str_adds.f              str_addi.f              str_addf.f \
sum.f                   isum.f                  reccel.f \
cellcx.f                extend_ldm.f            opn_scr.f \
scr_log.f               lsqmql.f                lsqsig.f \
dnls1_qqq.f             dfdjc3_qqq.f            dcov_qqq.f \
mtx_diag.f              mtx_solve_eqn.f         mtx_inv.f \
mtx_mult.f              mtx_mult_tr.f           mtx_negate.f \
mtx_pack.f              mtx_rotate.f            mtx_sca_mult.f \
mtx_unit.f              mtx_write.f             mtx_zero.f \
rowcol.f                pltsv.f                 stasav.f \
putsav.f                finsav.f                replot.f \
qginit.f                qgclos.f                qgentr.f \
qgclr.f                 qgtmod.f                qgbell.f \
qgsymb.f                qgmove.f                qgdraw.f \
qgtext.f                qgcurs.f                qgpset.f \
qgwrps.f                qgalph.f                qgxax1.f \
qgyax1.f                qgmapc.f                qgpixc.f \
qgmpxy.f                qgcirc.f                qsclra.f \
qshome.f                qscurl.f                qscurr.f \
qscuru.f                qscurd.f                qssetc.f \
qsclrc.f                qsclrl.f                qsscrl.f \
qsbell.f                qsnorv.f                qsrevv.f \
qgfont.f                qgsend.f                qgread.f \
trwset.f                tlog.f                  twrite.f \
tstrng.f                tpromp.f                taskq.f \
taskf.f                 taski.f                 tread.f \
centrc.f \
dml.c  kdm.c  lirl.c d1mach.c readpstiff.c


#liblaue_a_SOURCES = $(liblaue_la_SOURCES)

noinst_HEADERS = \
abscor_muin.finc   ldm_derived.finc   next_refl.finc     saint_limits.finc \
bg_progress.finc   ldm_extended.finc  next_refobs.finc   save_refpars.finc \
deconv.finc        ldm_lrl.finc       other_scale.finc   scale_presets.finc \
font.finc          lfn_findspots.finc prog_flags.finc    scp.finc \
found_pkpl.finc    lfn_saint.finc     ref_ctrl.finc      scp_doc.finc \
get_soft.finc      lfn_unq.finc       ref_flags.finc     scps_counters.finc \
implicit_none.finc limits.finc        refine_data.finc   screen_params.finc \
ksm_data.finc      lrl_data.finc      refine_limits.finc use_reflns.finc \
kwd_outstring.finc lrl_maxss.finc     refine_lsm.finc    view_handles.finc \
ldm_data.finc      lrl_nodmax.finc    refobs_list.finc   x_array.finc \
ldm_defaults.finc  lsm_flags.finc     rout_arrays.finc \
dml.h  dml_systyp.h  kdm.h  kdm_systyp.h  lirl.h  lirl_systyp.h \
readpstiff.h readpstiffP.h
