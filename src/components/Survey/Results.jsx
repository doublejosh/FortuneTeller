import React from 'react'
import { useSelector } from 'react-redux'
import { useFirebaseConnect } from 'react-redux-firebase'
import Logo from '../Logo'
import { css } from 'glamor'

export default props => {
	useFirebaseConnect(['fortunes'])
	let fortunes = useSelector(state => state.firebase.data.fortunes)

	const list = Object.keys(fortunes || {}).filter(f => fortunes[f].rank)

	let total = 0

	return fortunes ? (
		<section {...css({ margin: '1rem' })}>
			<Logo />
			<h3 {...css({ marginLeft: '1.5rem', color: 'rgba(0,0,0, .6)', fontSize: '2rem' })}>
				Survey results
			</h3>
			<ul {...css({ paddingLeft: '0', listStyle: 'none' })}>
				{list.map((k, i) => {
					let f = fortunes[k]
					total += f.rank.total
					return (
						<li
							key={i}
							{...css({
								'&.low-score': { opacity: 0.5 },
								padding: '.25rem 1rem',
								marginBottom: '.25rem',
								backgroundColor: 'rgba(255, 255, 255, .2)',
								display: 'grid',
								gridTemplateAreas: '"score fortune"',
								gridTemplateColumns: '6rem 1fr',
								opacity: (0.75 * f.rank.keep) / f.rank.total + 0.25,
							})}>
							{f.rank && (
								<React.Fragment>
									<div {...css({ fontSize: '1.75rem', gridArea: 'score' })}>
										{f.rank.total} &nbsp;
										{f.rank.keep && f.rank.total ? (
											<span {...css({ color: '#000' })}>
												{Math.round((f.rank.keep / f.rank.total) * 100)}%
											</span>
										) : (
											<span>0%</span>
										)}
									</div>
									<div
										{...css({
											display: 'grid',
											alignItems: 'center',
											fontSize: `${(0.75 * f.rank.keep) / f.rank.total + 1}rem`,
										})}>
										{f.text}
									</div>
								</React.Fragment>
							)}
						</li>
					)
				})}
			</ul>
			<div
				{...css({
					textAlign: 'right',
					marginTop: '1rem',
					fontSize: '3rem',
					lineHeight: '3rem',
					color: 'rgba(0, 0, 0, 0.6)',
				})}>
				<h2 {...css({ marginBottom: 0 })}>{total}</h2>
				<span>votes</span>
			</div>
		</section>
	) : null
}
