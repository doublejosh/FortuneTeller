import React from 'react'
import { useSelector } from 'react-redux'
import { useFirebaseConnect } from 'react-redux-firebase'
import { css } from 'glamor'

export default props => {
	useFirebaseConnect(['fortunes'])
	let fortunes = useSelector(state => state.firebase.data.fortunes)

	const list = Object.keys(fortunes || {}).filter(f => fortunes[f].rank)

	let total = 0

	return fortunes ? (
		<section {...css({ marginTop: '2rem' })}>
			<ul {...css({ paddingLeft: '0', margin: '0', listStyle: 'none' })}>
				{list.map((k, i) => {
					let f = fortunes[k]
					total += f.rank.total
					return (
						<li
							key={i}
							{...css({
								fontSize: '1.75rem',
								'&.low-score': { opacity: 0.5 },
							})}
							className={f.rank.keep / f.rank.total < 0.5 ? 'low-score' : ''}>
							{f.rank && (
								<React.Fragment>
									{f.rank.total} - {f.text}
									{f.rank.keep && (
										<span {...css({ color: '#000' })}>
											- {Math.round((f.rank.keep / f.rank.total) * 100)}%
										</span>
									)}
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
